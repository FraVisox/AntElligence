#include "board.h"

Board::Board(){ 
    reset();
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
        state = NOT_STARTED;
        moves.erase(moves.begin(), moves.end());
        G.reset();
        placedBug.erase(placedBug.begin(), placedBug.end());
        inHandPiece.clear();
    }


    /**
     * Converts the data of the board to the corresponding GameString.
     *
     * The GameString is the most detailed representation of the game state.
     *
     * :return: GameString.
     * :rtype: char*
     */
    char* Board::toString() {
        char* s = new char[40+moves.size()*9];
        sprintf(s, "%d;%d;%s[%d]", type, state, ColorToString(currentColor()), currentPlayerTurn());
        for (int i = 0; i < moves.size(); i++) {
            sprintf(s + strlen(s), ";%s", ActionToString(moves[i]));
        }
        return s;
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
        return 1+currentTurn/2;
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
    bool Board::executeAction(action a){

        //The move is represented as a Movestring (wS1 -wA1) or as "pass"

        if (state == NOT_STARTED) 
            state = IN_PROGRESS;
        switch (a.actType) {
            case MOVEMENT:
                a.startingPos = G.getPosition(a.bug);
                G.addPiece(a);
                G.removePiece(a.bug);
                moves.push_back(a);
                break;
            case PLACE:
                G.addPiece(a);
                placedBug.push_back(a.bug);
                inHandPiece.extract(a.bug);
                if(a.bug.kind==QUEEN){
                    if(currentColor()==WHITE)isPlacedWQ=true;
                    if(currentColor()==BLACK)isPlacedWQ=true;
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

        return checkWin();
    }

    bool Board::checkWin() {
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
            return true;
        } else if (black_surrounded) {
            state = WHITE_WIN;
            return true;
        } else if (white_surrounded) {
            state = BLACK_WIN;
            return true;
        }
        return false;
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
        if (moves.size() <= amount) {
            reset();
            return;
        }
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
        vector<action> res;

        // 1 if turn == 1, then place something that is not the queen
        if(currentTurn==1){
            for(piece b:inHandPiece){
                if(b.col==currentColor() && b.kind != QUEEN) {
                    res.push_back(placeFirst(b));
                }
            }
            return res;
        }

        // 2 if turn == 2 , then place something near white;
        if(currentTurn == 2){
            for(piece b: inHandPiece){
                if(b.col==currentColor() && b.kind != QUEEN){
                    for(direction dir : allDirections){
                        res.push_back(placePiece(b, placedBug[0], dir));
                    }
                }
            }
            return res;
        }

        // 3 - if i didn't place the queen after turn 3
        if(!placedQueen() && currentPlayerTurn()>3){
            auto positions = G.validPositionPlaceNew(currentColor());
            for(auto pos: positions){
                res.push_back(placePiece(piece{QUEEN,currentColor()},pos.first, pos.second));
            }
            return res;
        }
        
        // 4 - pieces in our hand
        vector<piece> inHandCol;
        for(auto p : inHandPiece){
            if(p.col==currentColor()){
                inHandCol.push_back(p);
            }
        }
        if(inHandCol.size()>0){
            auto positions=G.validPositionPlaceNew(currentColor());
            for(auto p:inHandCol) {
                for(auto pos : positions){
                    res.push_back(placePiece(p,pos.first, pos.second));
                }
            }
        }

        // 5- moves
        if(placedQueen()){
            piece mosq;
            piece pillb;
            for(piece b:placedBug){
                if (b.kind != PILLBUG && b.kind != MOSQUITO){ 
                    possibleMovesBug(b,res);
                } else if (b.kind == MOSQUITO){
                    mosq=b;
                } else {
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
        if(b.col==currentColor() && G.canPieceMove(b,currentTurn)){ // turn is required to make the program efficent
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
    void Board::possibleMoves_Queen(piece bug,vector<action> &res){
        for(position dest: G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getNearNeighbor(dest, G.getPosition(bug), false);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
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
        for(position dest:G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getNearNeighbor(dest, G.getPosition(bug), true);
            if (relativeDir.second != INVALID) {
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
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
                pair<piece,direction> relativeDir = G.getNearNeighbor(n, f, false);
                if (relativeDir.second != INVALID) {
                    res.push_back(movement(bug,relativeDir.first,relativeDir.second));
                    q.push(n);
                    inQueue.insert(n);
                }
            }
        }
        G.addPiece(startPos,bug);
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
                pair<piece,direction> relativeDir = G.getNearNeighbor(n, f, false);
                if (relativeDir.second != INVALID) {
                    if(d==3)
                        res.push_back(movement(bug,relativeDir.first,relativeDir.second));
                    else{
                        q.push(make_pair(n,d+1));
                        inQueue.insert(n);
                    }
                }
            }
        }
        G.addPiece(startPos,bug);
    }

    /* TODO: implement. DOES THE PILLBUG NEED TO CHECK FOR SLIDE FREE?

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

        // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
        for(position dest: G.getPosition(bug).neighbor()){
            pair<piece,direction> relativeDir = G.getNearNeighbor(dest, G.getPosition(bug), false);
            if (relativeDir.second != INVALID ) {//&& find(res.begin(), res.end(), movement(bug, relativeDir.first, relativeDir.second)) == res.end()){ todo
                res.push_back(movement(bug,relativeDir.first,relativeDir.second));
                
            }
        }

        // Or it makes other adjacent pieces (even of the opponent) move
        piece bugs_to_move[6];
        direction places_to_move[6];
        int i = 0;
        int j = 0;
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest) && G.isAtLevel1(dest)){
                piece possible_piece = G.topPiece(dest);
                if (G.canMoveWithoutBreakingHiveRule(possible_piece, currentTurn)){
                    pair<piece,direction> relativeDir = G.getNearNeighbor(G.getPosition(bug), dest, true);
                    if (relativeDir.second != INVALID) {
                        bugs_to_move[i] = possible_piece;
                        i++;
                    }
                }
            } else if (G.isFree(dest)){
                pair<piece,direction> relativeDir = G.getNearNeighbor(dest, G.getPosition(bug), true);
                if (relativeDir.second != INVALID) {
                    places_to_move[j] = getMovementDirection(G.getPosition(bug), dest);
                    j++;
                }
            }
        }
        if (i > 0 && j > 0){
            for(int k = 0; k < i; k++){
                for (int l = 0; l < j; l++){
                    piece to_move = bugs_to_move[k];
                    //Check if this movement is already inside of the vector
                    //if (find(res.begin(), res.end(), movement(to_move, bug, places_to_move[l])) == res.end()) TODO
                        res.push_back(movement(to_move, bug, places_to_move[l]));
                }
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
        if (!G.isAtLevel1(G.getPosition(bug))){
            //It can only move like a beetle
            possibleMoves_Beetle(bug,res);
        }

        set<BugType> copied;
        for(position n: G.getPosition(bug).neighbor()){
            if(!G.isFree(n) && G.topPiece(n).kind != MOSQUITO){
                copied.insert(G.topPiece(n).kind);
            }
        }

        bool pill = false;
        for(BugType k : copied){ 
            if (k == PILLBUG) {
                pill = true;
                continue;  
            }
            piece new_bug = bug;
            new_bug.kind = k;
            possibleMovesBug(new_bug, res);
        }
        //In this way, we are almost certain we don't have duplicates
        if (pill) {
            possibleMovesBug(bug, res);
        }
    }

    /**
     * Computes all the possible moves for a Ladybug piece.
     * A Ladybug can move three or two spaces in any direction, but the first 2 moves
     * should be over of the hive.
     * @param bug The piece to move.
     * @param res The vector where the possible moves are stored.
     */
    void Board::possibleMoves_Ladybug(piece bug,vector<action> &res){
        vector<pair<position,int>> queue; //TODO: HERE
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest)){
                pair<piece,direction> relativeDir = G.getNearNeighbor(dest, G.getPosition(bug), true);
                if (relativeDir.second != INVALID) {
                    queue.push_back(make_pair(dest, 1));
                }
            }
        }

        unordered_set<position> seen = unordered_set<position>{};
        seen.insert(G.getPosition(bug));
        while(!queue.empty()){
            pair<position,int> PI = queue.front();
            queue.erase(queue.begin());
            position f=PI.first;
            int d = PI.second;
            for (position n: f.neighbor()){
                if (seen.count(n) || (find(queue.begin(), queue.end(), make_pair(n, d)) != queue.end()) || (find(queue.begin(), queue.end(), make_pair(n, d+1)) != queue.end()))
                    continue;
                seen.insert(n);

                pair<piece,direction> relativeDir = G.getNearNeighbor(n, f, true);
                if (relativeDir.second != INVALID && G.isAtLevel1(G.getPosition(relativeDir.first).applayMove(relativeDir.second))) {
                    res.push_back(movement(bug, relativeDir.first, relativeDir.second));
                } else if (relativeDir.second != INVALID && d == 1) {
                    queue.push_back(make_pair(n, d + 1));
                }
            }
            seen.insert(f);
        }
    }