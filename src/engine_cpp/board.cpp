#include "board.h"
#include <regex>

/**
 * \brief Construct a new Board, initializing all members to default values.
 *
 * Initialize the new Board with all members set to default values.
 * The state of the board is set to NOT_STARTED, indicating that the game has not yet started.
 * The current turn is set to 1, indicating that it is White's turn to play.
 * The possible moves vector is cleared.
 * The game board and pieces are reset using the reset() method.
 * The isPlacedBQ and isPlacedWQ flags are set to false, indicating that neither the
 * black nor white queens have been placed on the board yet.
 */
Board::Board(){ 
    reset();
    state = NOT_STARTED;
    possibleMovesVector.clear();
};



/**
 * \brief Reset the board to the default state.
 *
 * Reset the board to the default state, initializing all members to default values.
 * This method is called by the constructor to initialize a new board.
 * The state of the board is set to STARTED, indicating that the game has started.
 * The current turn is set to 1, indicating that it is White's turn to play.
 * The possible moves vector is cleared.
 * The game board and pieces are reset using the reset() method.
 * The isPlacedBQ and isPlacedWQ flags are set to false, indicating that neither the
 * black nor white queens have been placed on the board yet.
 * All bugs that are currently on the board are placed back in the player's hand.
 * The placed bug vector is cleared.
 */
void Board::reset() {
    currentTurn = 1;
    state = STARTED;
    //We do not change the type of game
    moves.clear();
    possibleMovesVector.clear();
    isPlacedBQ = false;
    isPlacedWQ = false;
    G.reset();
    for (piece p : placedBug) {
        inHandPiece.insert(p);
    }
    placedBug.clear();
}

/**
 * \brief Copy the given board into this one.
 *
 * Copy the given board into this one, overwriting all the members of this
 * board with the corresponding members of the given board.
 *
 * \param b The board to copy from.
 */
void Board::copy(const Board& b) {
    currentTurn = b.currentTurn;
    type = b.type;
    state = b.state;
    moves = b.moves;
    possibleMovesVector = b.possibleMovesVector;
    //Copy the gameboard
    G.bugPosition = b.G.bugPosition;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            G.gb[i][j] = b.G.gb[i][j];
        }
    }
    G.occupied = b.G.occupied;
    placedBug = b.placedBug;
    inHandPiece = b.inHandPiece;
    isPlacedBQ = b.isPlacedBQ;
    isPlacedWQ = b.isPlacedWQ;
}



/**
 * \brief Convert the board to a string representation.
 *
 * This method converts the board into a string, which can be used to store the
 * current state of the game in a file or database, or to send it over a network.
 *
 * The string is a semicolon-separated list of the following elements:
 * - The game type (Base, Base_M, Base_L, Base_P, Base_ML, Base_MP, Base_LP, Base_MLP)
 * - The game state (NOT_STARTED, STARTED, IN_PROGRESS, DRAW, WHITE_WIN, BLACK_WIN)
 * - The current turn number, prefixed by "White[" or "Black[", depending on whose turn it is.
 * - A list of the moves made so far, each move being a MoveString.
 *
 * \return A string containing the current game state.
 */
string Board::toString() {
    try {
        stringstream ss;
        ss << GameTypeToString(type) << ";";
        ss << GameStateToString(state) << ";" 
        << (currentTurn % 2 == 1 ? "White[" : "Black[")
        << currentPlayerTurn() << "]";

        for (action move : moves) {
            ss << ";";
            ss << ActionToString(move);
        }
        
        return ss.str();
    }   
    catch (const std::exception& e) {
        return "ERROR";
}
}

/**
 * \brief Return the color of the current player.
 *
 * \return The color of the current player (WHITE or BLACK).
 */
PlayerColor Board::currentColor(){
    if(currentTurn%2==1) return PlayerColor::WHITE;
    return PlayerColor::BLACK;
}

/**
 * \brief Returns the turn number of the current player.
 *
 * The turn number increases every two moves, so the first move is turn 1, the third move is turn 2, and so on.
 *
 * \return The turn number of the current player.
 */
int Board::currentPlayerTurn(){
    return (currentTurn-1)/2+1;
}


/**
 * \brief Checks if the queen for the current player has been placed.
 *
 * \return True if the queen for the current player has been placed, false otherwise.
 */
bool Board::placedQueen(){
    if(currentColor()==WHITE) 
        return isPlacedWQ;
    return isPlacedBQ;
}


/**
 * \brief Execute a move in the game.
 *
 * Given a string representing a move, this function executes it in the current game.
 * The string should be a valid MoveString, which is a concatenation of a bug piece
 * to be moved and its relative position and direction. The relative position and
 * direction are represented by a single string, which is the concatenation of the
 * relative position bug piece and the direction indicator.
 *
 * \param s The string representing the move.
 * \return 0 if the move was successful, an error code otherwise.
 */
ReturnMessage Board::executeAction(string s){

    action a = validMove(s);
    if (a.bug == INVALID_PIECE) {
        return ERROR;
    }

    //The move is represented as a Movestring (wS1 -wA1) or as "pass"

    if (state == STARTED)
        state = IN_PROGRESS;
    else if (state != IN_PROGRESS) {
        return ERROR;
    }
    switch (a.actType) {
        case MOVEMENT:
            a.startingPos = G.getPosition(a.bug);
            G.removePiece(a.bug);
            G.addPiece(a);
            moves.push_back(a);
            break;
        case PLACE:
            G.addPiece(a);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            if(a.bug.kind==QUEEN){
                if(currentColor()==WHITE)isPlacedWQ=true;
                if(currentColor()==BLACK)isPlacedBQ=true;
            }
            moves.push_back(a);
            break;
        case PLACEFIRST:
            G.addPiece(a);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            moves.push_back(a);
            break;
        case PASS:
            moves.push_back(a);
            break;
    }
    currentTurn++;

    possibleMovesVector.clear();
    return checkWin();
}


/**
 * \brief Validates and parses a given move string.
 *
 * This function checks the syntax of a move string and determines if it is a valid move
 * for the current state of the board. It supports various actions such as passing, placing
 * the first piece, moving a piece, and special moves involving the mosquito and pillbug.
 *
 * If the move string is "pass", it checks if there are no valid moves available before allowing
 * the pass.
 *
 * For a placement move, it checks if the piece to be placed is in hand and if the position
 * is valid. It ensures that the first move is always made by a white piece.
 *
 * For a movement move, it validates if the piece to be moved is already placed on the board,
 * and ensures that the movement follows the rules for the specific bug type, including special
 * rules for the mosquito and pillbug.
 *
 * It also checks that the move does not repeat a previous movement action by the same piece
 * in consecutive turns.
 *
 * \param s The string representing the move.
 * \return An action object representing the parsed move, or INVALID_ACTION if the move is
 *         not valid.
 */
action Board::validMove(string s) {

    //Check the syntax of the string

    if (s == "pass") {
        if (possibleMoves().size() == 0) 
            return pass();
        return INVALID_ACTION;
    }

    std::regex pattern(R"(^(w|b)[QSBGAMLP](?:[1-3])?(?: ([\\\-/])(w|b)[QSBGAMLP](?:[1-3])?)?(?: (w|b)[QSBGAMLP](?:[1-3])?)?(?: (w|b)[QSBGAMLP](?:[1-3])?([\\\-/]))?$)");
    std::regex placeFirstPattern(R"(^(w|b)[QSBGAMLP](?:[1-3])?$)");
    
    if (!std::regex_match(s, pattern)) {
        return INVALID_ACTION;
    }

    if (!possibleMovesVector.empty()) {
        if (currentTurn == 1) {
            if (std::regex_match(s, placeFirstPattern)) {
                for (action a : possibleMoves()) {
                    if (ActionToString(a) == s) {
                        return a;
                    }
                }
            }
            else 
                return INVALID_ACTION;
        } else if (s.find(' ') == std::string::npos) {
            return INVALID_ACTION; //no second string
        }

        string first = s.substr(0, s.find(' '));
        string second = s.substr(s.find(' ') + 1);


        piece toMove = extractPiece(first);
        piece otherBug = extractPiece(second);
        direction d = extractDirection(second);

        position newPlace = G.getPosition(otherBug).applayMove(d);
        for (action a : possibleMoves()) {
            if (a.bug == toMove && newPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) {
                if (moves.size() > 1) {
                    action previous_move = moves[moves.size()-2];
                    if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                        return INVALID_ACTION;
                    }
                }
                return a;
            }
            if (toMove.kind == MOSQUITO && areBugsSameOrCopied(toMove, a.bug) && newPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) {
                a.bug = toMove;
                if (moves.size() > 1) {
                    action previous_move = moves[moves.size()-2];
                    if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                        return INVALID_ACTION;
                    }
                }
                return a;
            }
        }
        return INVALID_ACTION;
    }

    //Else we need to do everything by hand instead of recreating all the possible moves

    bool placeFirstMove = false;

    string first;

    if (currentTurn == 1) {
        if (std::regex_match(s, placeFirstPattern)) {
            placeFirstMove = true;
            first = s;
        }
        else {
            return INVALID_ACTION;
        }
    } else if (s.find(' ') != std::string::npos) {
        first = s.substr(0, s.find(' '));
    } else {
        return INVALID_ACTION; //no second string
    }


    //Check in inhandpiece to understand if the move is valid
    piece toMove = INVALID_PIECE;
    for (piece p : inHandPiece) {
        if (p.toString() == first) {
            toMove = p;
            break;
        }
    }

    //Place first piece
    if (placeFirstMove) {
        if (toMove == INVALID_PIECE || toMove.col != WHITE || toMove.kind == QUEEN) { //First to move is always the white
            return INVALID_ACTION;
        }
        return placeFirst(toMove);
    }

    //Check format of second string
    string second = s.substr(s.find(' ') + 1);


    piece otherBug = extractPiece(second);
    direction d = extractDirection(second);

    position otherBugPos = G.getPosition(otherBug);
    if (otherBugPos == NULL_POSITION) {
        return INVALID_ACTION;
    }
    position toPlace = otherBugPos.applayMove(d);

    //Place a piece
    if (toMove != INVALID_PIECE) {
        if (currentTurn == 2) {
            if (toMove.kind != QUEEN && toMove.col == BLACK && d != OVER) {
                return placePiece(toMove, otherBug, d);
            }
            return INVALID_ACTION;
        }
        if (G.isFree(toPlace) && toMove.col == currentColor()) {
            for (position neigh : toPlace.neighbor()) {
                if (!G.isFree(neigh) && G.topPiece(neigh).col != toMove.col) {
                    return INVALID_ACTION;
                }
            }
            return placePiece(toMove, otherBug, d);
        }
        return INVALID_ACTION;
    }

    //Move a piece
    if (currentColor() == WHITE && !isPlacedWQ) {
        return INVALID_ACTION;
    } else if (currentColor() == BLACK && !isPlacedBQ) {
        return INVALID_ACTION;
    }
    for (piece p : placedBug) {
        if (p.toString() == first) {
            toMove = p;
            break;
        }
    }
    if (toMove == INVALID_PIECE) {
        return INVALID_ACTION;
    }

    // Case where the pillbug moves the piece (of the opponent or not)
    position current = G.getPosition(toMove);
    if (G.isAtLevel1(current)) {
        vector<position> near = current.neighbor();
        for(position possiblePill : near){
            if(!G.isFree(possiblePill) && (G.topPiece(possiblePill).kind==PILLBUG && G.topPiece(possiblePill).col==currentColor())){
                if(G.isFree(toPlace) && (isNear(possiblePill,toPlace))) {
                    pair<piece, direction> relativeDir = G.getRelativePositionIfCanMove(possiblePill, current, true);
                    G.removePiece(toMove);
                    G.addPiece(possiblePill, toMove);
                    pair<piece, direction> relativeDir2 = G.getRelativePositionIfCanMove(toPlace, possiblePill, true);
                    G.removePiece(toMove);
                    G.addPiece(current, toMove);
                    if (relativeDir.second != INVALID && relativeDir2.second != INVALID) {
                        if (moves.size() > 1) {
                            action previous_move = moves[moves.size()-2];
                            if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                                return INVALID_ACTION;
                            }
                        }
                        return movement(toMove, otherBug, d);
                    }
                }
            } else if (!G.isFree(possiblePill) && G.topPiece(possiblePill).kind==MOSQUITO && G.topPiece(possiblePill).col==currentColor() && G.isAtLevel1(possiblePill)) {
                //Search between neighbors of the mosquito to see if the pillbug is near
                bool pillbug = false;
                for (position neigh : possiblePill.neighbor()) {
                    if (!G.isFree(neigh) && G.topPiece(neigh).kind==PILLBUG) {
                        pillbug = true;
                    }
                }
                if (pillbug && G.isFree(toPlace) && (isNear(possiblePill,toPlace))) {
                    pair<piece, direction> relativeDir = G.getRelativePositionIfCanMove(possiblePill, current, true);
                    G.removePiece(toMove);
                    G.addPiece(possiblePill, toMove);
                    pair<piece, direction> relativeDir2 = G.getRelativePositionIfCanMove(toPlace, possiblePill, true);
                    G.removePiece(toMove);
                    G.addPiece(current, toMove);
                    if (relativeDir.second != INVALID && relativeDir2.second != INVALID) {
                        if (moves.size() > 1) {
                            action previous_move = moves[moves.size()-2];
                            if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                                return INVALID_ACTION;
                            }
                        }
                        return movement(toMove, otherBug, d);
                    }
                }
            }
        }
    }

    //Otherwise it should be of the right color
    if (toMove.col != currentColor()) {
        return INVALID_ACTION;
    }

    //And the move should be done by the piece itself
    vector<action> res = vector<action>();
    possibleMovesBug(toMove, res); //TODO: remember to update both this and the one before
    for (action a : res) {
        if (a.bug == toMove && toPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) {
            if (moves.size() > 1) {
                action previous_move = moves[moves.size()-2];
                if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                    return INVALID_ACTION;
                }
            }
            return a;
        }
        if (toMove.kind == MOSQUITO && areBugsSameOrCopied(toMove, a.bug) && toPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) {
            a.bug = toMove;
            if (moves.size() > 1) {
                action previous_move = moves[moves.size()-2];
                if (previous_move.bug == toMove && previous_move.actType == MOVEMENT) {
                    return INVALID_ACTION;
                }
            }
            return a;
        }
   }
    return INVALID_ACTION;
}

/**
 * \brief Checks the win condition for the current game state.
 *
 * This function evaluates whether either player's queen is surrounded,
 * indicating a win or draw condition. It checks if both the white and
 * black queens have been placed on the board. If they have, it verifies
 * whether they are surrounded by other pieces.
 *
 * - If both queens are surrounded, the game is declared a draw.
 * - If only the black queen is surrounded, the white player wins.
 * - If only the white queen is surrounded, the black player wins.
 *
 * Updates the game state accordingly and returns a corresponding
 * ReturnMessage indicating the result of the check.
 *
 * \return A ReturnMessage indicating whether the game is ongoing,
 * a draw, or if a player has won.
 */

ReturnMessage Board::checkWin() {
    bool white_surrounded = false;
    bool black_surrounded = false;
    if (isPlacedBQ && isPlacedWQ) { //can't win without placing the queens
        for (piece p: placedBug) {
            if (p.kind == QUEEN) {
                if (p.col == WHITE && checkSurrounding(p)) {
                    white_surrounded = true;
                } else if (p.col == BLACK && checkSurrounding(p)) {
                    black_surrounded = true;
                }
            }
        }
    }
    if (black_surrounded && white_surrounded) {
        state = DRAW;
        return GAME_OVER_DRAW;
    } else if (black_surrounded) {
        state = WHITE_WIN;
        return GAME_OVER_WHITE_WINS;
    } else if (white_surrounded) {
        state = BLACK_WIN;
        return GAME_OVER_BLACK_WINS;
    }
    return OK;
}

/**
 * \brief Checks if a piece is surrounded by other pieces.
 *
 * Checks if all of a piece's neighbors are not free. If so, the
 * piece is considered surrounded and the method returns true.
 *
 * \param p The piece to check.
 * \return True if the piece is surrounded, false otherwise.
 */
bool Board::checkSurrounding(const piece &p) {
    position pos = G.getPosition(p);
    for (position adj: pos.neighbor()) {
        if (G.isFree(adj)) {
            return false;
        }
    }
    return true;
}

/**
 * \brief Undoes the specified amount of moves on the board.
 *
 * This function calls the undo method on the board to reverse the 
 * specified number of moves. It updates the game state accordingly.
 *
 * \param amount The number of moves to undo.
 */
void Board::undo(int amount) { 
    if (state == NOT_STARTED) {
        return;
    }
    if (moves.size() <= (long long unsigned int) amount) {
        moves.clear();
        reset();
        return;
    }
    if (state != IN_PROGRESS) {
        state = IN_PROGRESS;
    }
    possibleMovesVector.clear();
    for (int i = 0; i < amount; i++) {
        action move = moves.back();
        moves.pop_back();
        currentTurn--;
        //Pass
        if (move.actType == PASS) {
            continue;
        }
        //Placement
        if (move.actType == PLACE || move.actType == PLACEFIRST) {
            inHandPiece.insert(move.bug);
            placedBug.pop_back();
            if (move.bug == QUEEN) {
                if (currentColor() == WHITE) {
                    isPlacedWQ = false;
                } else {
                    isPlacedBQ = false;
                }
            }
            G.removePiece(move.bug);
        } else {
            //Movement
            G.removePiece(move.bug);
            G.addPiece(move.startingPos, move.bug);
        }
    }
}


/**
 * \brief Adds a piece to the in-hand pieces set.
 *
 * This method is used to add a piece to the set of pieces that are
 * currently in the player's hand. The piece is added to the set
 * of in-hand pieces, which is used to determine which pieces can
 * be placed on the board.
 *
 * \param p The piece to add to the in-hand pieces set.
 */
void Board::addPieceHand(piece p){
    inHandPiece.insert(p);
}

/**
 * \brief Get all possible moves for the current player.
 *
 * This function returns all possible moves for the current player.
 * It returns a vector of action objects, each representing a possible
 * move. The moves are determined by the current state of the game and
 * the pieces on the board.
 *
 * The function first checks if the board is empty, and if it is, it
 * returns an empty vector. Then it checks if the current player has
 * already placed their queen, and if they have, it returns a vector
 * containing all possible moves for the current player. If the current
 * player has not placed their queen, it returns a vector containing
 * all possible moves for the current player, including placing the
 * queen.
 *
 * If the current player has placed all their pieces, the function
 * returns a vector containing all possible moves for the current
 * player, including passing.
 *
 * If the current player has placed some pieces, but not all of them,
 * the function returns a vector containing all possible moves for
 * the current player, including placing pieces and passing.
 *
 * If the current player has not placed any pieces, the function
 * returns a vector containing all possible moves for the current
 * player, including placing pieces and passing.
 *
 * \return A vector of action objects, each representing a possible
 *         move for the current player.
 */
vector<action> Board::possibleMoves(){
    if (!possibleMovesVector.empty()) {
        return possibleMovesVector;
    }

    vector<action> res;

    // 1 If turn == 1, then place something that is not the queen
    if(currentTurn==1){
        for(piece b:inHandPiece){
            if(b.col==currentColor() && b.kind != QUEEN && (b.numIncr == 0 || b.numIncr == 1)) {
                res.push_back(placeFirst(b));
            }
        }
        return res;
    }

    // 2 If turn == 2 , then place something near white;
    if(currentTurn == 2){
        for(piece b: inHandPiece){
            if(b.col==currentColor() && b.kind != QUEEN && (b.numIncr == 0 || b.numIncr == 1)){
                for(direction dir : allDirections){
                    res.push_back(placePiece(b, placedBug[0], dir));
                }
            }
        }
        return res;
    }

    // 3 - If i didn't place the queen after turn 3, it's the only move
    if(!placedQueen() && currentPlayerTurn()>3){
        auto positions = G.validPositionPlaceNew(currentColor());
        for(auto pos: positions){
            res.push_back(placePiece(piece{QUEEN,currentColor()},pos.first, pos.second));
        }
        return res;
    }
    
    // 4 - Place pieces in our hand
    piece inHandCol[8] = {INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE};
    bool toPlace = false;
    for(auto p : inHandPiece){
        if(p.col==currentColor()){
            if (inHandCol[p.kind] == INVALID_PIECE || inHandCol[p.kind].numIncr > p.numIncr){ 
                inHandCol[p.kind] = p;
                toPlace = true;
            }
        }
    }
    if(toPlace){
        auto positions=G.validPositionPlaceNew(currentColor());
        for(auto p:inHandCol) {
            if (p == INVALID_PIECE) {
                continue;
            }
            for(auto pos : positions){
                res.push_back(placePiece(p, pos.first, pos.second));
            }
        }
    }

    // 5- moves
    if(placedQueen()){
        piece mosq = INVALID_PIECE;
        piece pillb = INVALID_PIECE;

        piece previous = INVALID_PIECE;
        if (moves.size() > 1)
            previous = moves[moves.size()-2].bug;
        for(piece b:placedBug){
            if (b == previous) continue;
            if (b.kind != PILLBUG && b.kind != MOSQUITO){ 
                possibleMovesBug(b,res);
            } else if (b.kind == MOSQUITO && b.col == currentColor()){
                mosq=b;
            } else if (b.col == currentColor()) {
                pillb=b;
            }
        }
        //In this way, the mosquito and the pillbug are the last to move
        possibleMovesBug(mosq,res);
        possibleMovesBug(pillb,res);
    }
    return res;
}


void Board::possibleMovesBug(piece b, vector<action> &res){
    if (b == INVALID_PIECE) return;
    if(b.col==currentColor()){ // turn is required to make the program efficent
        switch(b.kind){
            case BEETLE:
                possibleMoves_Beetle(b,res);
                break;
            case QUEEN:
                possibleMoves_Queen(b,res);
                break;
            case GRASSHOPPER:
                possibleMoves_Grasshopper(b,res);
                break;
            case SOLDIER_ANT:
                possibleMoves_SoldierAnt(b,res);
                break;
            case SPIDER:
                possibleMoves_Spider(b,res);
                break;
            case MOSQUITO:
                possibleMoves_Mosquito(b,res);
                break;
            case LADYBUG:
                possibleMoves_Ladybug(b,res);
                break;
            case PILLBUG:
                possibleMoves_Pillbug(b,res);
                break;
        }
    }
}



void Board::possibleMoves_Queen(piece bug, vector<action> &res){
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, false);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
            }
        }
    }
}


void Board::possibleMoves_Beetle(piece bug,vector<action> &res){
    if (G.canPieceMove(bug, currentTurn)) {
        for(position dest : G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, true);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
            }
        }
    }
}

void Board::possibleMoves_Grasshopper(piece bug,vector<action> &res){
    if (G.canPieceMove(bug,currentTurn)) {
        position from=G.getPosition(bug);
        for(direction dir : allDirections){
            position next = from.applayMove(dir);
            position old_next = next;
            if(!G.isFree(next)){
                do{
                    old_next = next;
                    next=next.applayMove(dir);
                } while(!G.isFree(next));
                res.push_back(movement(bug, G.at(old_next)->top(), getMovementDirection(old_next, next)));
            }
        }
    }
}


void Board::possibleMoves_SoldierAnt(piece bug, vector<action> & res){
    if (G.canPieceMove(bug,currentTurn)) {

        unordered_set<position> inQueue;
        queue<position> q;

        position startPos=G.getPosition(bug);
        q.push(startPos);
        inQueue.insert(startPos);
        G.removePiece(bug);

        while(!q.empty()){
            position f=q.front();
            q.pop();
            for(auto n:f.neighbor()){
                if(inQueue.count(n)) 
                    continue;
                pair<piece,direction> relativeDir = G.getSlidingMoveAtLevel1(n, f);
                if (relativeDir.second != INVALID) {
                    res.push_back(movement(bug,relativeDir.first,relativeDir.second));
                    q.push(n);
                    inQueue.insert(n);
                }
            }
        }
        G.addPiece(startPos,bug);
    }
}


void Board::possibleMoves_Spider(piece bug, vector<action> & res){
    if (G.canPieceMove(bug,currentTurn)) {
        unordered_set<position> inQueue;
        queue<pair<position,int>> q;

        position startPos=G.getPosition(bug);
        q.push(make_pair(startPos,0));
        inQueue.insert(startPos);
        G.removePiece(bug);

        while(!q.empty()){
            pair<position,int> PI = q.front();
            q.pop();
            position f=PI.first;
            int d = PI.second;
            for(auto n:f.neighbor()){
                if(inQueue.count(n)) 
                    continue;
                pair<piece,direction> relativeDir = G.getSlidingMoveAtLevel1(n, f);
                if (relativeDir.second != INVALID) {
                    if(d==2) {
                        res.push_back(movement(bug,relativeDir.first,relativeDir.second));
                    }
                    else{
                        q.push(make_pair(n,d+1));
                        inQueue.insert(n);
                    }
                }
            }
        }
        G.addPiece(startPos,bug);
    }
}

void Board::possibleMoves_Pillbug(piece bug, vector<action> &res){

    if (!G.isAtLevel1(G.getPosition(bug))) {
        return;
    }

    // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, false);
            //TODO: is it necessary to check inside for duplicate moves?
            if (relativeDir.second != INVALID ) { 
                bool already_in = false;
                for (auto pair : res){
                    if (pair.bug == bug && G.getPosition(pair.otherBug).applayMove(pair.relativeDir) == G.getPosition(relativeDir.first).applayMove(relativeDir.second)){
                        already_in = true;
                        break;
                    }
                }
                if (!already_in) {
                    res.push_back(movement(bug, relativeDir.first, relativeDir.second));
                }
            }
        }
    }

    // Or it makes other adjacent pieces (even of the opponent) move
    piece bugs_to_move[6];
    position places_to_move[6];
    int i = 0;
    int j = 0;
    for(position dest: G.getPosition(bug).neighbor()){
        if(!G.isFree(dest) && G.isAtLevel1(dest)){
            piece possible_piece = G.topPiece(dest);
            if (G.canMoveWithoutBreakingHiveRule(possible_piece, currentTurn)){
                //If valid, this piece can be moved over the pillbug
                position pos = G.getPosition(bug);
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(pos, dest, true);
                if (relativeDir.second != INVALID) {
                    bugs_to_move[i] = possible_piece;
                    i++;
                }
            }
        } else if (G.isFree(dest)){
            places_to_move[j] = dest;
            j++;
        }
    }
    if (i > 0 && j > 0){
        for(int k = 0; k < i; k++){
            piece to_move = bugs_to_move[k];
            position old = G.getPosition(to_move);
            G.removePiece(to_move);
            G.addPiece(G.getPosition(bug), to_move); //already checked this can be done
            for (int l = 0; l < j; l++){
                position pos = G.getPosition(bug);
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(places_to_move[l], pos, false);
                if (relativeDir.second != INVALID) {
                    //Check if this movement is already inside of the vector TODO: is it necessary to check for duplicate moves????
                    bool already_in = false;
                    for (auto pair : res){
                        if (pair.bug == to_move && G.getPosition(pair.otherBug).applayMove(pair.relativeDir) == G.getPosition(relativeDir.first).applayMove(relativeDir.second)){
                            already_in = true;
                            break;
                        }
                    }
                    if (!already_in) {
                        res.push_back(movement(to_move, relativeDir.first, relativeDir.second));
                    }
                }
            }
            G.removePiece(to_move);
            G.addPiece(old, to_move);

        }
    }
}


void Board::possibleMoves_Mosquito(piece bug, vector<action> &res){
    if (!G.canPieceMove(bug, currentTurn)) 
        return;
    if (!G.isAtLevel1(G.getPosition(bug))){
        //It can only move like a beetle
        possibleMoves_Beetle(bug,res);
        return;
    }

    set<BugType> copied;
    for(position n: G.getPosition(bug).neighbor()){
        if(!G.isFree(n) && G.topPiece(n).kind != MOSQUITO){
            copied.insert(G.topPiece(n).kind);
        }
    }

    bool pill = false;
    position current = G.getPosition(bug);
    G.removePiece(bug);
    for(BugType k : copied){ 
        if (k == PILLBUG) {
            pill = true;
            continue;  
        }
        piece new_bug = piece(-1); //this signals the bug is copied
        new_bug.kind = k;
        new_bug.col = bug.col;
        G.addPiece(current, new_bug);
        possibleMovesBug(new_bug, res);
        G.removePiece(new_bug);
    }
    //In this way, we are almost certain we don't have duplicates
    if (pill) {
        piece new_bug = piece(-1); //this signals the bug is copied
        new_bug.kind = PILLBUG;
        new_bug.col = bug.col;
        G.addPiece(current, new_bug);
        possibleMovesBug(new_bug, res);
        G.removePiece(new_bug);
    }
    G.addPiece(current, bug);
}

void Board::possibleMoves_Ladybug(piece bug,vector<action> &res){
    if (G.canPieceMove(bug,currentTurn)) {
        vector<pair<position,int>> queue; 
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest)){
                position pos = G.getPosition(bug);
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, true);
                if (relativeDir.second != INVALID) {
                    queue.push_back(make_pair(dest, 1));
                }
            }
        }

        //The set of seen positions is used to avoid going to the same destination twice. 
        //It contains only the starting position and the final ones (if we put also the intermediate ones, then we can't reach the final
        //destinations reachable if I go on another piece and then move to the piece as a second move and then descending)
        unordered_set<position> seen = unordered_set<position>{};
        seen.insert(G.getPosition(bug));
        position current = G.getPosition(bug);
        G.removePiece(bug);
        while(!queue.empty()){
            pair<position,int> PI = queue.front();
            queue.erase(queue.begin());
            position f=PI.first;
            int d = PI.second;
            G.addPiece(f, bug); //Add the bug on top of f to see if it can move
            for (position n: f.neighbor()){
                if (seen.count(n))
                    continue;

                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(n, f, true);
            
                if (relativeDir.second != INVALID) {
                    if (G.isFree(n) && d == 2 && !seen.count(n)) { //TODO: It must do exactly 3 steps right?
                        res.push_back(movement(bug, relativeDir.first, relativeDir.second));
                        seen.insert(n);
                    } else if (d == 1 && !G.isFree(n)) {
                        queue.push_back(make_pair(n, d + 1));
                    }
                } 
            }
            G.removePiece(bug);
        }
        G.addPiece(current, bug);
    }
}