
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

    Board(){ 
        currentTurn=1;
    };


    PlayerColor currentColor(){  // we start with 1
        if(currentTurn%2==1) return PlayerColor::WHITE;
        return PlayerColor::BLACK;
    }
    
    bool placedQueen(){
        if(currentColor()==WHITE)return isPlacedWQ;
        return isPlacedBQ;
    }

    vector<action> possibleMoves(){
        vector<action> res;

        // 1 if turn == 1, then place something that is not the queen
        if(currentTurn==1){
            for(piece b:inHandPiece){
                if(b.col==currentColor() && b.kind!=QUEEN){
                    res.push_back(placeFirst(b));
                }
            }
            return res;
        }

        // 2 if turn ==2 , then place something near white;
        if(currentTurn==2){
            for(piece b:inHandPiece){
                if(b.col==currentColor() && b.kind!=QUEEN){
                    for(pair<int,int> dir:movementCircleClockwise){
                        res.push_back(placePiece(b,position{dir.first,dir.second}));
                    }
                }
            }
            return res;
        }

        // 3 - if i didn't place the queen after turn 5

        if(!placedQueen() && currentTurn>8){
            auto positions=G.validPositionPlaceNew(currentColor());
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
            for(piece b:placedBug){
                possibleMoves_Bug(b,res);
            }
        }
        return res;
    
    }

    void possibleMoves_Bug(piece b,vector<action> &res){
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
                    possibleMoves_SoldierAnd(b,res);
                    break;
                case SPIDER:
                    possibleMoves_Spider(b,res);
                    break;
                case MOSQUITO:
                    possibleMoves_Mosquito(b,res);
                    break;
                case LADYBUG:
                case PILLBUG:
                break;
            }
        }
    }


    void possibleMoves_Queen(piece bug,vector<action> &res){
        for(position dest:G.getPosition(bug).neighbor()){
            if(G.canSlideFree(G.getPosition(bug),dest)){
                res.push_back(movement(bug,dest));
            }
        }
    }

    void possibleMoves_Beetle(piece bug,vector<action> &res){
        for(position dest:G.getPosition(bug).neighbor()){
            if(G.canSlideFree(G.getPosition(bug),dest) || G.isFree(dest)){
                res.push_back(movement(bug,dest));
            }
        }
    }

    void possibleMoves_Grasshopper(piece bug,vector<action> &res){
        position from=G.getPosition(bug);
        for(Direction dir : allDirections){
            position next=from.applayMove(dir);
            if(!G.isFree(next)){
                do{
                    next=next.applayMove(dir);

                }while(!G.isFree(next));
                res.push_back(movement(bug,next));
            }
        }
    }


    void possibleMoves_SoldierAnd(piece bug, vector<action> & res){
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
                if(inQueue.count(n)) continue;
                if(G.canSlideFree(f,n)){
                    res.push_back(movement(bug,n));
                    q.push(n);
                    inQueue.insert(n);
                }
            }
        }
        G.addPiece(startPos,bug);
    }



    void possibleMoves_Spider(piece bug, vector<action> & res){
        unordered_set<position> inQueue;
        queue<pair<position,int>> q;
        position startPos=G.getPosition(bug);
        q.push(make_pair(startPos,0));
        inQueue.insert(startPos);
        G.removePiece(bug);

        while(!q.empty()){
            pair<position,int> PI=q.front();
            q.pop();
            position f=PI.first;
            int d=PI.second;
            for(auto n:f.neighbor()){
                if(inQueue.count(n)) continue;
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

    


    void possibleMoves_Mosquito(piece b,vector<action> &res){
        set<BugType> copied;
        for(position n: G.getPosition(b).neighbor()){
            if(!G.isFree(n)){
                copied.insert(G.topPiece(n).kind);
            }
        }
        piece bOriginal=b;
        position initialPos=G.getPosition(b);  // TODO: COMPLETE
        for(BugType k : copied){   
            G.removePiece(bOriginal);
            b.kind=k;
            G.addPiece(initialPos,b);

        }
    }


    void executeAction(action a){
        switch (a.actType)
        {
        case MOVEMENT:
            G.removePiece(a.bug);
            G.addPiece(a.pos,a.bug);
            break;
        case PLACE:
            G.addPiece(a.pos,a.bug);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            if(a.bug.kind==QUEEN){
                if(currentColor()==WHITE)isPlacedWQ=true;
                if(currentColor()==BLACK)isPlacedWQ=true;
            }
            break;
        case PLACEFIRST:
            G.addPiece(position{0,0},a.bug);
            placedBug.push_back(a.bug);
            inHandPiece.extract(a.bug);
            break;
        case PASS:
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

    void addPieceHand(piece p){
        inHandPiece.insert(p);
    }


    
};