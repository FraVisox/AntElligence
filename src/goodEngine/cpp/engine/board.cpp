#include "board.h"
#include "../graph_board/update_board.h"
#include <regex>


#ifndef BOARD_CPP
#define BOARD_CPP

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

    currentTurn = 1;
    state = STARTED;
    //We do not change the type of game
    inHandPiece.reset();
    prevMoved[0]=0;
    prevMoved[1]=0;
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

/**
 * \brief Copy the given board into this one.
 *
 * Copy the given board into this one, overwriting all the members of this
 * board with the corresponding members of the given board.
 *
 * \param b The board to copy from.
 */
/*
    QUEEN=0,
    SPIDER=1,
    BEETLE=2,
    GRASSHOPPER=3,
    SOLDIER_ANT=4,
    MOSQUITO=5,
    LADYBUG=6,
    PILLBUG=7
*/

int kindValue[] = {100, 10, 70, 40, 90, 35, 60, 45};

int Board::getScoreBug(pieceT p) {
    int multipl = kindValue[kind(p)];
    if (isPinned(p) || isCovered(p)) {
        return -multipl;
    }
    return multipl*(friendlyNeighbour(p)-enemyNeighbour(p));
}

int Board::getScore(PlayerColor color) {
    int res = 0;
    if (G.isPlaced[22] && G.isPlaced[8]) {
        for (pieceT p=1;p<=28;p++) {
            if(G.isPlaced[p]){
                if (col(p) == color) {
                    res += getScoreBug(p);
                } else {
                    res -= getScoreBug(p);
                }
            }
        }
    }
    return res;
}



int Board::countSurrounding(pieceT p) {
    position pos = G.getPosition(p);
    int ret = 0;
    for (position adj: pos.neighbor()) {
        if (!G.isFree(adj)) {
            ret += 1;
        }
    }
    return ret;
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

void Board::copy(Board &b){
    this->G.copy(b.G);
    this->currentTurn=b.currentTurn;
    this->inHandPiece=b.inHandPiece;
    this->numAction=b.numAction;
    this->prevMoved[0]=b.prevMoved[0];
    this->prevMoved[1]=b.prevMoved[1];
    for(int i=0;i<256;i++)
        this->resAction[i]=b.resAction[i];
    this->state=b.state;
    this->type=b.type;
}



/**
 * \brief Checks if the queen for the current player has been placed.
 *
 * \return True if the queen for the current player has been placed, false otherwise.
 */
bool Board::placedQueen(){
    if(currentColor()==WHITE) 
        return G.isPlaced[8];
    return G.isPlaced[22];
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

/*ReturnMessage Board::executeAction(string s){

   // action a = validMove(s);
   // if (a == INVALID_ACTION) {
   //     return INVALID_ARGUMENT;
   // }
   action a = parseAction(s, inHandPiece);

    //The move is represented as a Movestring (wS1 -wA1) or as "pass"

    if (state == STARTED)
        state = IN_PROGRESS;
    else if (state != IN_PROGRESS) {
        return INVALID_GAME_NOT_STARTED;
    }
    switch (a.actType) {
        case MOVEMENT:
            a.startingPos = G.getPosition(a.bug);
            G.removePiece(a.bug);
            G.addPiece(a);
            break;
        case PLACE:
            G.addPiece(a);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            if(kind(a.bug)==QUEEN){
                if(currentColor()==WHITE)isPlacedWQ=true;
                if(currentColor()==BLACK)isPlacedBQ=true;
            }
            break;
        case PLACEFIRST:
            G.addPiece(a);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            break;
        case PASS:
            break;
    }
    currentTurn++;

    return checkWin();
}

ReturnMessage Board::checkWin() {
    bool white_surrounded = false;
    bool black_surrounded = false;
    if (isPlacedBQ && isPlacedWQ) { //can't win without placing the queens
        for (pieceT p: placedBug) {
            if (kind(p) == QUEEN) {
                if (col(p) == WHITE && checkSurrounding(p)) {
                    white_surrounded = true;
                } else if (col(p) == BLACK && checkSurrounding(p)) {
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
}*/

/**
 * \brief Checks if a piece is surrounded by other pieces.
 *
 * Checks if all of a piece's neighbors are not free. If so, the
 * piece is considered surrounded and the method returns true.
 *
 * \param p The piece to check.
 * \return True if the piece is surrounded, false otherwise.
 */
/*
bool Board::checkSurrounding(const pieceT &p) {
    position pos = G.getPosition(p);
    for (position adj: pos.neighbor()) {
        if (G.isFree(adj)) {
            return false;
        }
    }
    return true;
}*/
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

void Board::addPieceHand(pieceT p){
    inHandPiece.set(p,1);
}

void Board::applayAction(actionT a){
    UpdateBoardE(*this,a);

}


/**
 * 
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


void Board::ComputePossibleMoves(){

    numAction=0;    
    // 1 If turn == 1, then place something that is not the queen
    if(currentTurn==1){
        for(int b=0;b<32;b++){
            if(inHandPiece[b]){
            if(col(b)==currentColor() && kind(b) != QUEEN && (numInc(b) == 0 || numInc(b) == 1)) {
                resAction[numAction]=placeFirst(b);
                numAction++;
            }
            }
        }
        return;
    }

    // 2 If turn == 2 , then place something near white;
    if(currentTurn == 2){
        for(int  b=0;b<32;b++){
            if(inHandPiece[b]){
            if(col(b)==currentColor() && kind(b) != QUEEN && (numInc(b) == 0 || numInc(b) == 1)){
                for(direction dir : allDirections){
                    resAction[numAction]=placePiece(b, prevMoved[0]+prevMoved[1], dir);
                    numAction++;
                }
            }
            }
        }
        return;
    }

    // 3 - If i didn't place the queen after turn 3, it's the only move
    if(!placedQueen() && currentPlayerTurn()>3){
        auto positions = G.validPositionPlaceNew(currentColor());
        for(auto pos: positions){
            resAction[numAction]=placePiece(buildPiece(QUEEN,currentColor()),pos.first, pos.second);
            numAction++;
        }
        return;
    }
    
    // 4 - Place pieces in our hand
    pieceT inHandCol[8] = {INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE};
    bool toPlace = false;
    for(int p=0;p<32;p++){
        if(inHandPiece[p]){
        if(col(p)==currentColor()){
            if (inHandCol[kind(p)] == INVALID_PIECE || numInc(inHandCol[kind(p)]) > numInc(p)){ 
                inHandCol[kind(p)] = p;
                toPlace = true;
            }
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
                resAction[numAction]=(placePiece(p, pos.first, pos.second));
                numAction++;
            }
        }
    }

    // 5- moves
    if(placedQueen()){
        pieceT mosq = INVALID_PIECE;
        pieceT pillb = INVALID_PIECE;

        pieceT previous = INVALID_PIECE;

        for(pieceT b=1;b<=28;b++){
            if(!G.isPlaced[b])continue;
            if (b == prevMoved[0] || b == prevMoved[1]) continue;
            if (kind(b) != PILLBUG && kind(b) != MOSQUITO){ 
                possibleMovesBug(b);
            } else if (kind(b) == MOSQUITO && col(b) == currentColor()){
                mosq=b;
            } else if (col(b) == currentColor()) {
                pillb=b;
            }
        }
        possibleMovesBug(mosq);
        possibleMovesBug(pillb);
    }
    return;
}


void Board::possibleMovesBug(pieceT b){
    if (b == INVALID_PIECE) return;
    if(col(b)==currentColor()){ // turn is required to make the program efficent
        switch(kind(b)){
            case BEETLE:
                possibleMoves_Beetle(b);
                break;
            case QUEEN:
                possibleMoves_Queen(b);
                break;
            case GRASSHOPPER:
                possibleMoves_Grasshopper(b);
                break;
            case SOLDIER_ANT:
                possibleMoves_SoldierAnt(b);
                break;
            case SPIDER:
                possibleMoves_Spider(b);
                break;
            case MOSQUITO:
                possibleMoves_Mosquito(b);
                break;
            case LADYBUG:
                possibleMoves_Ladybug(b);
                break;
            case PILLBUG:
                possibleMoves_Pillbug(b);
                break;
        }
    }
}



void Board::possibleMoves_Queen(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, false);
            if (relativeDir.second != INVALID) {
                resAction[numAction]=(movement(bug,relativeDir.first,relativeDir.second));
                numAction++;
            }
        }
    }
}


void Board::possibleMoves_Beetle(pieceT bug){
    if (G.canPieceMove(bug, currentTurn)) {
        for(position dest : G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, true);
            if (relativeDir.second != INVALID) {
                resAction[numAction]=(movement(bug,relativeDir.first,relativeDir.second));
                numAction++;
            }
        }
    }
}

void Board::possibleMoves_Grasshopper(pieceT bug){
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
                resAction[numAction]=(movement(bug, G.topPiece(old_next), getMovementDirection(old_next, next)));
                numAction++;
            }
        }
    }
}


void Board::possibleMoves_SoldierAnt(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {

        bitset<SIZE_BOARD*SIZE_BOARD> inQueue;
        inQueue.reset();
        queue<position> q;

        position startPos=G.getPosition(bug);
        q.push(startPos);
        inQueue.set(startPos.toInt(),1);
        G.removePiece(bug);

        while(!q.empty()){
            position f=q.front();
            q.pop();
            for(auto n:f.neighbor()){
                if(inQueue[n.toInt()]) 
                    continue;
                pair<pieceT,direction> relativeDir = G.getSlidingMoveAtLevel1(n, f);
                if (relativeDir.second != INVALID) {
                    resAction[numAction]=(movement(bug,relativeDir.first,relativeDir.second));
                    numAction++;
                    q.push(n);
                    inQueue.set(n.toInt(),1);
                }
            }
        }
        G.addPiece(startPos,bug);
    }
}


void Board::possibleMoves_Spider(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        bitset<SIZE_BOARD*SIZE_BOARD> inQueue;
        inQueue.reset();
        queue<pair<position,int>> q;

        position startPos=G.getPosition(bug);
        q.push(make_pair(startPos,0));
        inQueue.set(startPos.toInt(),1);
        G.removePiece(bug);

        while(!q.empty()){
            pair<position,int> PI = q.front();
            q.pop();
            position f=PI.first;
            int d = PI.second;
            for(auto n:f.neighbor()){
                if(inQueue[n.toInt()]) 
                    continue;
                pair<pieceT,direction> relativeDir = G.getSlidingMoveAtLevel1(n, f);
                if (relativeDir.second != INVALID) {
                    if(d==2) {
                        resAction[numAction]=(movement(bug,relativeDir.first,relativeDir.second));
                        numAction++;
                    }
                    else{
                        q.push(make_pair(n,d+1));
                        inQueue.set(n.toInt(),1);
                    }
                }
            }
        }
        G.addPiece(startPos,bug);
    }
}

void Board::possibleMoves_Pillbug(pieceT bug){

    if (!G.isAtLevel1(G.getPosition(bug))) {
        return;
    }

    // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
    if (G.canPieceMove(bug,currentTurn)) {
        for(position dest: G.getPosition(bug).neighbor()){
            position pos = G.getPosition(bug);
            pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, false);
            //TODO: is it necessary to check inside for duplicate moves?
            if (relativeDir.second != INVALID ) { 
                bool already_in = false;
                
                for (int iAct=0;iAct<numAction;iAct++){
                    action pair=resAction[iAct];
                    if (pair.bug == bug && G.getPosition(pair.otherBug).applayMove(pair.relativeDir) == G.getPosition(relativeDir.first).applayMove(relativeDir.second)){
                        already_in = true;
                        break;
                    }
                }
                if (!already_in) {
                    resAction[numAction]=(movement(bug, relativeDir.first, relativeDir.second));
                    numAction++;
                }
            }
        }
    }

    // Or it makes other adjacent pieces (even of the opponent) move
    pieceT bugs_to_move[6];
    position places_to_move[6];
    int i = 0;
    int j = 0;
    for(position dest: G.getPosition(bug).neighbor()){
        if(!G.isFree(dest) && G.isAtLevel1(dest)){
            pieceT possible_piece = G.topPiece(dest);
            if (G.canMoveWithoutBreakingHiveRule(possible_piece, currentTurn)){
                //If valid, this piece can be moved over the pillbug
                position pos = G.getPosition(bug);
                pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(pos, dest, true);
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
            pieceT to_move = bugs_to_move[k];
            position old = G.getPosition(to_move);
            G.removePiece(to_move);
            G.addPiece(G.getPosition(bug), to_move); //already checked this can be done
            for (int l = 0; l < j; l++){
                position pos = G.getPosition(bug);
                pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(places_to_move[l], pos, false);
                if (relativeDir.second != INVALID) {
                    //Check if this movement is already inside of the vector TODO: is it necessary to check for duplicate moves????
                    bool already_in = false;
                    for (int iAct=0;iAct<numAction;iAct++){
                        action pair=resAction[iAct];
                        if (pair.bug == to_move && G.getPosition(pair.otherBug).applayMove(pair.relativeDir) == G.getPosition(relativeDir.first).applayMove(relativeDir.second)){
                            already_in = true;
                            break;
                        }
                    }
                    if (!already_in) {
                        resAction[numAction]=(movement(to_move, relativeDir.first, relativeDir.second));
                        numAction++;
                    }
                }
            }
            G.removePiece(to_move);
            G.addPiece(old, to_move);

        }
    }
}


void Board::possibleMoves_Mosquito(pieceT bug){
    if (!G.canPieceMove(bug, currentTurn)) 
        return;
    if (!G.isAtLevel1(G.getPosition(bug))){
        //It can only move like a beetle
        possibleMoves_Beetle(bug);
        return;
    }

    bitset<10> copied;
    for(position n: G.getPosition(bug).neighbor()){
        if(!G.isFree(n) && kind(G.topPiece(n)) != MOSQUITO){
            copied.set(kind(G.topPiece(n)),1);
        }
    }

    bool pill = false;
    position current = G.getPosition(bug);
    G.removePiece(bug);
    for(int kBT=0;kBT<10;kBT++){
        if(copied[kBT]){
            BugType k=(BugType)kBT;
            if (k == PILLBUG) {
                pill = true;
                continue;  
            }
            pieceT new_bug = 29; //this signals the bug is copied
            isSimulatingPiece=true,
            simulatedPiece = getCandidateForKind(k,col(bug));
            G.addPiece(current, new_bug);
            possibleMovesBug(new_bug);
            G.removePiece(new_bug);
            isSimulatingPiece=false;
        }
    }
    //In this way, we are almost certain we don't have duplicates
    if (pill) {
        pieceT new_bug = 29; //this signals the bug is copied
        isSimulatingPiece=true;
        simulatedPiece=getCandidateForKind(BugType::PILLBUG,col(bug));
        G.addPiece(current, new_bug);
        possibleMovesBug(new_bug);
        G.removePiece(new_bug);
    }
    G.addPiece(current, bug);
}

void Board::possibleMoves_Ladybug(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        vector<pair<position,int>> queue; 
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest)){
                position pos = G.getPosition(bug);
                pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(dest, pos, true);
                if (relativeDir.second != INVALID) {
                    queue.push_back(make_pair(dest, 1));
                }
            }
        }

        //The set of seen positions is used to avoid going to the same destination twice. 
        //It contains only the starting position and the final ones (if we put also the intermediate ones, then we can't reach the final
        //destinations reachable if I go on another piece and then move to the piece as a second move and then descending)
        bitset<SIZE_BOARD*SIZE_BOARD> seen;
        seen.reset();
        seen.set(G.getPosition(bug).toInt(),1);
        position current = G.getPosition(bug);
        G.removePiece(bug);
        while(!queue.empty()){
            pair<position,int> PI = queue.front();
            queue.erase(queue.begin());
            position f=PI.first;
            int d = PI.second;
            G.addPiece(f, bug); //Add the bug on top of f to see if it can move
            for (position n: f.neighbor()){
                if (seen[n.toInt()])
                    continue;

                pair<pieceT,direction> relativeDir = G.getRelativePositionIfCanMove(n, f, true);
            
                if (relativeDir.second != INVALID) {
                    if (G.isFree(n) && d == 2 && !seen[n.toInt()]) { //TODO: It must do exactly 3 steps right?
                        resAction[numAction]=(movement(bug, relativeDir.first, relativeDir.second));
                        numAction++;
                        seen.set(n.toInt(),1);
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



int Board::isPinned(pieceT bug) {
    return !G.canPieceMove(bug, currentTurn);
}

int Board::isCovered(pieceT bug) {
    return !G.isTop(bug);
}
int Board::friendlyNeighbour(pieceT bug) {
    int res = 0;
    for (position adj: G.getPosition(bug).neighbor()) {
        if (G.isFree(adj) || col(G.topPiece(adj)) == col(bug)) {
            res++;
        }
    }
    return res;
}
int Board::enemyNeighbour(pieceT bug) {
    int res = 0;
    for (position adj: G.getPosition(bug).neighbor()) {
        if (!G.isFree(adj) && col(G.topPiece(adj)) != col(bug)) {
            res++;
        }
    }
    return res;
}


#endif