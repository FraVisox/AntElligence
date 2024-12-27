
#include "gameboard.cpp"
#include "action.cpp"

class Board {
    public:
    int currentTurn;
    gameboard G;
    vector<piece> placedBug;
    unordered_set<piece> inHandPiece;
    bool isPlacedWQ=false;
    bool isPlacedBQ=false;

    Board():
        placedBug{},
        G(&placedBug){ currentTurn=1;};


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
                    res.push_back(placeFirst{b});
                }
            }
            return res;
        }

        // 2 if turn ==2 , then place something near white;
        if(currentTurn==2){
            for(piece b:inHandPiece){
                if(b.col==currentColor() && b.kind!=QUEEN){
                    for(pair<int,int> dir:movementCircleClockwise){
                        res.push_back(placePiece{b,position{dir.first,dir.second}});
                    }
                }
            }
            return res;
        }

        // 3 - if i didn't place the queen after turn 5

        if(!placedQueen() && currentTurn>8){
            auto positions=G.validPositionPlaceNew(currentColor());
            for(position pos:positions){
                placePiece m(piece{QUEEN,currentColor()},pos);
                res.push_back(m);
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
                    res.push_back(placePiece{p,pos});
                }
        }

        // 2- moves
        if(placedQueen()){
            for(int i=0;i<placedBug.size();i++){
                piece p=placedBug[i];
                if(p.col==currentColor() && G.canPieceMove(i,currentTurn)){ // turn is required to make the program efficent
                    switch(p.kind){
                        case BEETLE:
                            possibleMoves_Beetle(i,res);
                            break;
                        case QUEEN:
                            possibleMoves_Queen(i,res);
                            break;
                        case SPIDER:
                        case GRASSHOPPER:
                            possibleMoves_Grasshopper(i,res);
                            break;
                        case SOLDIER_ANT:
                        case MOSQUITO:
                        case LADYBUG:
                        case PILLBUG:
                        break;
                    }
                }
            }
        }

    }
    void possibleMoves_Queen(int bug,vector<action> &res){
        for(position dest:placedBug[bug].placed.neighbor()){
            if(G.canSlideFree(placedBug[bug].placed,dest)){
                res.push_back(movement{placedBug[bug],dest});
            }
        }
    }

    void possibleMoves_Beetle(int bug,vector<action> &res){
        for(position dest:placedBug[bug].placed.neighbor()){
            if(G.canSlideFree(placedBug[bug].placed,dest) || G.isFree(placedBug[bug].placed)){
                res.push_back(movement{placedBug[bug],dest});
            }
        }
    }

    void possibleMoves_Grasshopper(int bug,vector<action> &res){
        position from=placedBug[bug].placed;
        for(Direction dir : allDirections){
            position next=from.applayMove(dir);
            if(!G.isFree(next)){
                do{
                    next=next.applayMove(dir);

                }while(!G.isFree(next));
                res.push_back(movement{placedBug[bug],next});
            }
        }
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
    
    
    void movePiece(piece p,position end){
        if(placement.count(p)){
            position pos=placement.at(p);
            if(gameboard.at(pos).top()!=p)
                throw "This piece cannoto move, is not in the top";
            gameboard.at(pos).pop();
            if(gameboard.at(pos).empty()){
                occupied_position.erase(pos);
            }
        }

        gameboard.at(end).push(p);
        placement.insert_or_assign(p,end);
        occupied_position.insert(end);
        */
    }

    
};