
#include "gameboard.cpp"
#include "action.cpp"
#include <unordered_set>
#include <queue>

class Board {
    public:
    
    int currentTurn;
    gameboard G;
    vector<piece> placedBug;
    unordered_set<piece> inHandPiece;
    bool isPlacedWQ=false;
    bool isPlacedBQ=false;

    GameState state;

    int type; //Base, BASE+MLP...

    vector<action> moves; //moves made

    /**
     * Board constructor.
     *
     * Initializes the Board with the white player going first.
     * The turn number is set to 1.
     */
    Board(){ 
        currentTurn=1;
    };


    /**
     * Converts the data of the board to the corresponding GameString.
     *
     * The GameString is the most detailed representation of the game state.
     *
     * :return: GameString.
     * :rtype: char*
     */
    char* toString() {
        char* s = new char[1000];
        sprintf(s, "%d;%s;%s[%d];", type, state, ColorToString(currentColor()), currentPlayerTurn());
        for (int i = 0; i < moves.size(); i++) {
            sprintf(s + strlen(s), "%s;", ActionToString(moves[i]));
        }
        return s;
    }

    /**
     * Undoes the specified amount of moves.
     *
     * :param amount: Amount of moves to undo.
     * :type amount: int
     * :raises ValueError: If there are not enough moves to undo.
     * :raises ValueError: If the game has yet to begin.
     */
    void undo(int amount) {
        for (int i = 0; i < amount; i++) {
            if (moves.size() == 0) {
                break;
            }
            action move = moves.back();
            moves.pop_back();
            currentTurn--;
            if (move.actType == PASS) {
                continue;
            }
            if (move.actType == PLACE || move.actType == PLACEFIRST) {
                //Placement
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
                G.addPiece(move.pos, move.bug);
            }
        }
    }


    /**
     * Returns the current player color (white if the current turn is odd).
     *
     * @return The current player color.
     */
    PlayerColor currentColor(){
        if(currentTurn%2==1) return PlayerColor::WHITE;
        return PlayerColor::BLACK;
    }

    int currentPlayerTurn(){
        return 1+currentTurn%2;
    }

    
    /**
     * Returns whether the current player has placed their queen or not.
     *
     * @return Whether the current player has placed their queen or not.
     */
    bool placedQueen(){
        if(currentColor()==WHITE) 
            return isPlacedWQ;
        return isPlacedBQ;
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
    vector<action> possibleMoves(){
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
                    for(pair<int,int> dir:movementCircleClockwise){
                        res.push_back(placePiece(b,position{dir.first,dir.second}));
                    }
                }
            }
            return res;
        }

        // 3 - if i didn't place the queen after turn 3

        if(!placedQueen() && currentTurn>6){
            auto positions = G.validPositionPlaceNew(currentColor());
            for(position pos:positions){
                res.push_back(placePiece(piece{QUEEN,currentColor()},pos));
            }
            return res;
        }
        
        // 4 - pieces in our hand
        vector<piece> inHandCol;
        for(auto p:inHandPiece){
            if(p.col==currentColor()){
                inHandCol.push_back(p);
            }
        }
        if(inHandCol.size()>0){
            auto positions=G.validPositionPlaceNew(currentColor());
            for(auto p:inHandCol)
                for(auto pos : positions){
                    res.push_back(placePiece(p,pos));
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

    void possibleMovesBug(piece b, vector<action> &res){
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
    void possibleMoves_Queen(piece bug,vector<action> &res){
        for(position dest: G.getPosition(bug).neighbor()){
            if(G.canSlideFree(G.getPosition(bug),dest)){
                res.push_back(movement(bug,dest));
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
    void possibleMoves_Beetle(piece bug,vector<action> &res){
        for(position dest:G.getPosition(bug).neighbor()){
            //if(G.canSlideFree(G.getPosition(bug),dest) || G.isFree(dest)){        TODO: check
                res.push_back(movement(bug,dest));
            //}
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
    void possibleMoves_Grasshopper(piece bug,vector<action> &res){
        position from=G.getPosition(bug);
        for(Direction dir : allDirections){
            position next = from.applayMove(dir);
            if(!G.isFree(next)){
                do{
                    next=next.applayMove(dir);
                } while(!G.isFree(next));
                res.push_back(movement(bug,next));
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
    void possibleMoves_SoldierAnt(piece bug, vector<action> & res){
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
                if(G.canSlideFree(f,n)){
                    res.push_back(movement(bug,n));
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
    void possibleMoves_Spider(piece bug, vector<action> & res){
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
                if(G.canSlideFree(f,n)){
                    if(d==3)
                        res.push_back(movement(bug,n));
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
    void possibleMoves_Pillbug(piece bug, vector<action> &res){

        // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
        for(position dest: G.getPosition(bug).neighbor()){
            if(G.canSlideFree(G.getPosition(bug),dest) && find(res.begin(), res.end(), movement(bug, dest)) == res.end()){
                res.push_back(movement(bug,dest));
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
                    bugs_to_move[i] = possible_piece;
                    i++;
                }
            } else if (G.isFree(dest)){
                places_to_move[j] = dest;
                j++;
            }
        }
        if (i > 0 && j > 0){
            for(int k = 0; k < i; k++){
                for (int l = 0; l < j; l++){
                    piece to_move = bugs_to_move[k];
                    //Check if this movement is already inside of the vector
                    if (find(res.begin(), res.end(), movement(to_move, places_to_move[l])) == res.end())
                        res.push_back(movement(to_move, places_to_move[l]));
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
    void possibleMoves_Mosquito(piece bug, vector<action> &res){
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
    void possibleMoves_Ladybug(piece bug,vector<action> &res){
        vector<pair<position,int>> queue;
        for(position dest: G.getPosition(bug).neighbor()){
            if(!G.isFree(dest)){
                queue.push_back(make_pair(dest, 1));
            }
        }

        unordered_set<position> seen;
        seen.insert(G.getPosition(bug));
        while(!queue.empty()){
            pair<position,int> PI = queue.front();
            queue.erase(queue.begin());
            position f=PI.first;
            int d = PI.second;
            for (position n: f.neighbor()){
                if (seen.count(n) || find(queue.begin(), queue.end(), n) != queue.end())
                    continue;
                seen.insert(n);

                if (G.isFree(n)) {
                    res.push_back(movement(bug, n));
                } else if (d == 1) {
                    queue.push_back(make_pair(n, d + 1));
                }
            }
            seen.insert(f);
        }
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
    void executeAction(action a){
        if (state != IN_PROGRESS) 
            state = IN_PROGRESS;
        switch (a.actType)
        {
        case MOVEMENT:
            G.removePiece(a.bug);
            G.addPiece(a.pos,a.bug);
            moves.push_back(a);
            break;
        case PLACE:
            G.addPiece(a.pos,a.bug);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            if(a.bug.kind==QUEEN){
                if(currentColor()==WHITE)isPlacedWQ=true;
                if(currentColor()==BLACK)isPlacedWQ=true;
            }
            moves.push_back(a);
            break;
        case PLACEFIRST:
            G.addPiece(position{0,0},a.bug);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            moves.push_back(a);
            break;
        case PASS:
            moves.push_back(a);
            break;
        }
        currentTurn++;
    }
                    
    


    bool isValidMovement(piece &p, position &dest){ // TODO, not important now
        return true;
        /*
        position from = placement.at(p);

        // case where the pillbug move our piece
        vector<piece> near=getNeighborPiece(p);
        for(auto b:near){
            if(b.kind==PILLBUG && b.col==currentTurn){
                if(isFree(dest) && 
                    (isNear(from,dest)) && 
                    gameboard.at(from).size()==1)
                    return true;
                else
                    break;
            }
        }

        if(p.col!=currentTurn)return false;
        


        switch (p.kind)
        {
            case QUEEN:
                if(!isFree(dest)) return false;
                if(isNear(from,dest)) return true;
                return false;
            case BEETLE:
                if(isNear(from,dest)) return true;
                return false;
            case SPIDER:
                return "S";
            case GRASSHOPPER:
                return "G";
            case SOLDIER_ANT:
                return "A";
            case MOSQUITO:
                return "M";
            case LADYBUG:
                return "L";
            case PILLBUG:
                return "P";
        }
    }
    */
    }

    /**
     * \brief Get the attachment position of a piece on the board.
     *
     * Determines the position relative to other pieces on the board where 
     * a given piece is attached. If the given position is free, it checks 
     * all directions for an adjacent occupied position and returns a 
     * string representation of the attachment direction. If the position 
     * is not connected to any other piece, it throws an exception.
     *
     * \param pos The position to check.
     * \return A string representing the attachment direction of the piece.
     * \throws std::string if the given position is disconnected from the graph.
     */
    string getAttachPosition(position pos){
        if(G.isFree(pos)){
            for(Direction d:allDirections){
                position n=pos.applayMove(oppositeDir(d));
                if(!G.isFree(n)){
                    return stringNameDir(G.topPiece(pos).toString(),d);
                }
            }
            throw "Not valid position: disconnected from graph";
        }

        return G.topPiece(pos).toString();
    }

    /**
     * \brief Adds a piece to the player's hand.
     *
     * Inserts the given piece into the in-hand piece set, indicating that the
     * piece is available for the player to use in future actions.
     * 
     * \param p The piece to add to the player's hand.
     */
    void addPieceHand(piece p){
        inHandPiece.insert(p);
    }


    
};