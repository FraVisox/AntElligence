#include <bits/stdc++.h>
#include "position.cpp"
#include "piece.cpp"


class Board {
    public:
    int currentTurn;
    unordered_map<position, stack<piece>> gameboard;
    unordered_map<piece, position> placement;
    vector<piece> bugs;
    unordered_set<position> occupied_position;
    unordered_set<piece> not_played; 
    

    Board(){

    };

    void addPiece(piece p){
        bugs.push_back(p);
    }
    PlayerColor currentColor(){
        if(currentTurn%2==1) return PlayerColor::WHITE;
        return PlayerColor::BLACK;
    }


    bool singleHiveWithout(const piece &p){
        if(placement.count(p)){
            position pos=placement.at(p);
            if(gameboard.at(pos).size()>1) return true;
            if(not_movable_position.count(pos))  return false;
            
        }
        return true;
    }
    
    vector<piece> getNeighborPiece(const piece &p){
        position pos=placement.at(p);
        vector<piece> piecesNear;
        for(auto N:pos.neighbor()){
            if(occupied_position.count(N))
                piecesNear.push_back(gameboard.at(N).top());
        }
        return piecesNear;
    }
    

    piece getTopPiece(const position &p){
        return gameboard.at(p).top();   
    }

    bool isTopPiece(const piece &p){
        position pos=placement.at(p);
        return gameboard.at(pos).top()==p;
    }
    bool isPlaced(const piece &p){
        return placement.count(p);
    }
    bool isFree(const position &pos){
        return !occupied_position.count(pos);
    }
    bool isOccupied(const position &pos){
        return !isFree(pos);
    }

    bool canPieceMoveFrom(piece p){
        if(!isPlaced(p)) throw "Asking to move a piece not in the gameboard";
        if(!isTopPiece(p))return false;
        position pos=placement.at(p);
        if(gameboard.at(pos).size()>1) return true;
        if(singleHiveWithout(p)) return true;
        return false;
    }

    bool canPieceMove(piece &p,position &to){
        if(!canPieceMoveFrom(p)) return false;
        if(isValidMovement(p,to))return true;
        return false;
    }

    unordered_set<position> validPositionPlaceNew(PlayerColor color){
        unordered_set<position> seen;
        unordered_set<position> valid;
        for(auto op :occupied_position){
            piece p=gameboard.at(op).top();
            if(p.col==color){
                for(auto possiblePosition:op.neighbor()){
                    if(seen.count(possiblePosition)) break;
                    seen.insert(possiblePosition);
                    if(isFree(possiblePosition)){
                        bool hasOtherColor=false;
                        for(auto n:possiblePosition.neighbor()){
                            if(isOccupied(n) && gameboard.at(n).top().col!=color){
                                hasOtherColor=true;
                                break;
                            }
                        }
                        if(!hasOtherColor){
                            valid.insert(possiblePosition);
                        }
                    }
                }
            }
        }
        return valid;
    }


    bool canSlideTo(piece p,position pos){
        
    }

    vector<pair<piece,position>> possibleMoves(){
        vector<pair<piece,position>> res;

        // 0 - if i didn't place the queen after turn 5

        if(currentTurn>8 && currentTurn<=10&& not_played.size()>0){
            piece Q(BugType::QUEEN,currentColor());
            if(not_played.count(Q)){
                for(auto pos:validPositionPlaceNew(currentColor())){
                    res.push_back(make_pair(Q,pos));
                }
            }
        }

        // 1 - pieces in our hand
        vector<piece> in_hand;

        for(auto p:not_played){
            if(p.col==currentColor()){
                if(currentTurn>2 || p.kind!=QUEEN){  // case first turn queen
                    in_hand.push_back(p);
                }
            }
        }
        if(in_hand.size()>0){
            auto positions=validPositionPlaceNew(currentColor());
            for(auto p:in_hand)
                for(auto pos : positions){
                    res.push_back(make_pair(p,pos));
                }
        }

        // 2- moves
        for(piece p : bugs){
            // moves pillpub

            // TODO


            // moves other bugs
            if(not_played.count(p)==0 &&p.col==currentColor() && canPieceMoveFrom(p)){
                position piecePosition=placement.at(p);
                vector<position> nearPosition=piecePosition.neighbor();
                switch(p.kind){
                    case QUEEN:
                        for(position pos : nearPosition){
                            if(isFree(pos) && isConnectedWithout(pos,p)){
                                res.push_back(make_pair(p,pos));
                            }
                        }
                        break;
                    case BEETLE:
                        for(position pos : nearPosition){
                            res.push_back(make_pair(p,pos));
                        }
                    case SPIDER:
                    case GRASSHOPPER:
                    case SOLDIER_ANT:
                    case MOSQUITO:
                    case LADYBUG:
                    case PILLBUG:
                }
            }
        }

    }

    bool isValidMovement(piece &p, position &dest){ // TODO, not important now
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
    }

    void addPiece(piece p, position pos){
        movePiece(p,pos);
    }



    int turnVisitUnmovable=0;
    int timer=0;
    unordered_set<position> not_movable_position;
    unordered_set<position> visited_dfs;
    unordered_map<position,int> tin,low;

    void update_find_unmovable(){
        if(currentTurn==1){
            not_movable_position.clear();
            return;
        }
        if(currentTurn==turnVisitUnmovable) return;
        turnVisitUnmovable=currentTurn;
        not_movable_position.clear();

        visited_dfs.clear();
        for(position e:occupied_position){
            tin[e]=-1;
            low[e]=-1;
        }
        position startPos=*(occupied_position.begin());
        dfs(startPos);
    }


    void dfs(position &v, position &p = NULL_POSITION) {
        visited_dfs.insert(v);
        tin[v] = low[v] = timer++;
        int children=0;
        
        for (position to : v.neighbor()) {
            if(occupied_position.count(to)==0)continue;
            if (visited_dfs.count(to)) {
                low[v] = min(low[v], tin[to]);
            } else {
                dfs(to, v);
                low[v] = min(low[v], low[to]);
                if (low[to] >= tin[v] && p!=NULL_POSITION)
                    not_movable_position.insert(v);
                ++children;
            }
        }
        if(p == NULL_POSITION && children > 1)
            not_movable_position.insert(v);
    }






};