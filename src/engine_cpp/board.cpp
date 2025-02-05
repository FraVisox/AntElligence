#include "board.h"
#include <iostream>
#include <regex>

Board::Board(){ 
    reset();
    state = NOT_STARTED;
    possibleMovesVector.clear();
};


/**
 * Resets the board to its initial state.
 *
 * Sets the turn number to 1, game type to Base, and game state to NOT_STARTED.
 * Clears all recorded moves, placed bugs, and pieces in hand.
 * Resets the gameboard to its default configuration.
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

void Board::copy(Board& b) {
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
 * Converts the data of the board to the corresponding GameString.
 *
 * The GameString is the most detailed representation of the game state.
 *
 * :return: GameString.
 * :rtype: char*
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
 * Returns the current player color (white if the current turn is odd).
 *
 * @return The current player color.
 */
PlayerColor Board::currentColor(){
    if(currentTurn%2==1) return PlayerColor::WHITE;
    return PlayerColor::BLACK;
}

/**
 * Returns the turn number of the current player.
 *
 *
 * @return The turn number of the current player.
 * @see currentColor
 */
int Board::currentPlayerTurn(){
    return (currentTurn-1)/2+1;
}


/**
 * Returns whether the current player has placed their queen or not.
 *
 * @return Whether the current player has placed their queen or not.
 */
bool Board::placedQueen(){
    if(currentColor()==WHITE) 
        return isPlacedWQ;
    return isPlacedBQ;
}


/**
 * Executes an action. The action can be of type MOVEMENT, PLACE, PLACEFIRST
 * or PASS. Depending on the type of the action, the bug is moved, placed
 * or removed from the board, the turn is incremented, and the
 * placed/available bug pieces are updated. If the action is a PASS, nothing
 * is done.
 *
 * \param a The action to be executed.
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
                return a;
            }
            if (toMove.kind == MOSQUITO && areBugsSameOrCopied(toMove, a.bug) && newPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) { //TODO: bug con mosquito, se la muovo poi ha delle mosse sbagliate, come se non si fosse mossa
                a.bug = toMove;
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
            return a;
        }
        if (toMove.kind == MOSQUITO && areBugsSameOrCopied(toMove, a.bug) && toPlace == G.getPosition(a.otherBug).applayMove(a.relativeDir)) { //TODO: bug con mosquito, se la muovo poi ha delle mosse sbagliate, come se non si fosse mossa
            a.bug = toMove;
            return a;
        }
   }
    return INVALID_ACTION;
}

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

bool Board::checkSurrounding(piece p) {
    position pos = G.getPosition(p);
    for (position adj: pos.neighbor()) {
        if (G.isFree(adj)) {
            return false;
        }
    }
    return true;
}

/**
 * Undoes the specified amount of moves.
 *
 * :param amount: Amount of moves to undo.
 * :type amount: int
 * :raises ValueError: If there are not enough moves to undo.
 * :raises ValueError: If the game has yet to begin.
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
 * Returns a vector of all possible moves for the current player.
 *
 * Rules to generate moves:
 *
 * 1. If turn == 1, then place something that is not the queen.
 * 2. If turn == 2, then place something near white.
 * 3. If i didn't place the queen after turn 3, place the queen in any valid position.
 * 4. Pieces in our hand.
 * 5. Moves.
 *
 * @return A vector of all possible moves for the current player.
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
        for(piece b:placedBug){
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


/**
 * \brief Adds a piece to the player's hand.
 *
 * Inserts the given piece into the in-hand piece set, indicating that the
 * piece is available for the player to use in future actions.
 * 
 * \param p The piece to add to the player's hand.
 */
void Board::addPieceHand(piece p){
    inHandPiece.insert(p);
}

/**
 * \brief Generates all possible moves for a specific bug.
 *
 * This function calculates all the possible moves for the given bug based
 * on its type. If the bug belongs to the current player and can move without
 * violating the hive rules, it delegates to specific functions to generate
 * moves relevant to the bug's type. The generated moves are added to the 
 * provided result vector.
 *
 * \param b The bug for which to generate possible moves.
 * \param res The vector to store the resulting actions/moves.
 */

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


/**
 * Generates all valid moves for a Queen bug piece. The Queen can move
 * to any empty adjacent position, as long as the hive rules are not
 * violated. The generated moves are added to the provided result vector.
 *
 * \param bug The Queen bug piece for which to generate moves.
 * \param res The vector to store the resulting actions/moves.
 */
void Board::possibleMoves_Queen(piece bug, vector<action> &res){
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, G.getPosition(bug), false);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
            }
        }
    }
}

/**
 * Generates all valid moves for a Beetle bug piece. The Beetle can move
 * to any adjacent position, both if it's empty or not
 * as long as the hive rules are not violated. The generated moves are
 * added to the provided result vector.
 *
 * \param bug The Beetle bug piece for which to generate moves.
 * \param res The vector to store the resulting actions/moves.
 */
void Board::possibleMoves_Beetle(piece bug,vector<action> &res){
    if (G.canPieceMove(bug, currentTurn)) {
        for(position dest : G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, G.getPosition(bug), true);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
            }
        }
    }
}

/**
 * Generates all valid moves for a Grasshopper bug piece. The Grasshopper
 * can move to any position that is in the same direction as the bug
 * is currently at. The generated moves are added to the provided result
 * vector.
 * \param bug The Grasshopper bug piece for which to generate moves.
 * \param res The vector to store the resulting actions/moves.
 */
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


/**
 * Generates all valid moves for a Soldier Ant bug piece. The Soldier Ant
 * can move to any position that is reachable by moving one or more
 * tiles in any direction. The generated moves are added to the provided
 * result vector. The algorithm used is a breadth-first search to find
 * all reachable positions.
 *
 * \param bug The Soldier Ant bug piece for which to generate moves.
 * \param res The vector to store the resulting actions/moves.
 */
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

/**
 * Generates all valid moves for a Spider bug piece. The Spider can move to
 * any position that is reachable by moving three tiles in any
 * direction. The generated moves are added to the provided result vector.
 * The algorithm used is a breadth-first search to find all reachable
 * positions.
 *
 * \param bug The Spider bug piece for which to generate moves.
 * \param res The vector to store the resulting actions/moves.
 */
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

/*

Ladybug: 1 climb, then either 1 crawl or 1 climb, then 1 crawl into an unoccupied space. 
        The Ladybug must start and end its move at height 1, but it cannot be at height 
        1 at any other time while moving.
Mosquito: if its height is 1, it has the abilities of any uncovered non-Mosquito pieces in occupied adjacent spaces. 
        If it is only adjacent to uncovered Mosquitoes, it has no abilities. 
        If its height is greater than 1, it has only the abilities of a Beetle.
Pill bug: either 1 crawl or cause an adjacent piece at height 1  
        to perform 1 climb to the same space as the Pillbug, then 1 crawl to an unoccupied destination space.
*/    

/**
 * Computes all the possible moves for a Pillbug piece.
 * A Pillbug can move as a queen, or it can move another adjacent piece (even of the opponent) one space, as long as it doesn't break the hive.
 * @param bug The piece to move.
 * @param res The vector where the possible moves are stored.
 */
void Board::possibleMoves_Pillbug(piece bug, vector<action> &res){

    if (!G.isAtLevel1(G.getPosition(bug))) {
        return;
    }

    /*
    either 1 crawl or cause an adjacent piece at height 1  
    to perform 1 climb to the same space as the Pillbug, then 1 crawl to an unoccupied destination space.
    
    */

    // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, G.getPosition(bug), false);
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
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(G.getPosition(bug), dest, true);
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
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(places_to_move[l], G.getPosition(bug), false);
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

/**
 * Generates all valid moves for a Mosquito bug piece. The Mosquito can
 * mimic the movement abilities of neighboring bug pieces, except other
 * Mosquitoes. If the Mosquito is not at level 1, it can only move like a Beetle.
 * It also has the ability to perform special moves if it copies a Pillbug's
 * movement abilities. The generated moves are added to the provided result
 * vector.
 *
 * @param bug The Mosquito bug piece for which to generate moves.
 * @param res The vector to store the resulting actions/moves.
 */
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

/**
 * Computes all the possible moves for a Ladybug piece.
 * A Ladybug can move three or two spaces in any direction, but the first 2 moves
 * should be over of the hive.
 * @param bug The piece to move.
 * @param res The vector where the possible moves are stored.
 */
void Board::possibleMoves_Ladybug(piece bug,vector<action> &res){
    if (G.canPieceMove(bug,currentTurn)) {
        vector<pair<position,int>> queue; 
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest)){
                pair<piece,direction> relativeDir = G.getRelativePositionIfCanMove(dest, G.getPosition(bug), true);
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