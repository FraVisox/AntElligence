#include <set>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "piece.h"
#include "position.h"
#include "enums.h"
using namespace  std;

class gameboard{
    public:


    stack<piece> gb[100][100];  // stack of where a vector is placed
    unordered_map<piece,position> bugPosition; 
    unordered_set<position> occupied;

    gameboard(){}

    stack<piece>* at(position pos){
        return &gb[(100+(pos.first%100))%100][(100+(pos.second%100))%100];
    }

    
    position getPosition(piece p){
        return bugPosition.at(p);
    }
    void updatePos(piece bug,const position &pos){
        bugPosition.insert_or_assign(bug,pos);
    }


    void popPosition(position pos){
        if(!isFree(pos)){
            updatePos(at(pos)->top(),NULL_POSITION);
            at(pos)->pop();
        }
        if(isFree(pos))
            occupied.extract(pos);
    }


    void removePiece(piece b){
        if(isTop(b)){
            popPosition(getPosition(b));
            bugPosition.extract(b);
        }
    }

    void addPiece(position pos,piece b){
        at(pos)->push(b);
        updatePos(b,pos);
        occupied.insert(pos);
    }

    bool isFree(position &pos){
        return at(pos)->empty();
    }

    bool isTop(piece bug){
        position pos=getPosition(bug);
        return (at(pos)->top()==bug);
    }

    bool canPieceMove(piece b,int turn){
        return (isTop(b) && hiveRuleWithout(b,turn));
    }

    bool canSlideFree(position from, position to){
        if(isFree(to) && isNear(to, from)){
            vector<position> v=otherWay(from,to);
            position p1=v[0],p2=v[1];
            if(isFree(p1) ^ isFree(p2)) return true;
        }
        return false;
    }

    piece topPiece(position pos){
        if(!isFree(pos)){
            return at(pos)->top();
        }
        throw "Asked piece for empty pos";
    }
    unordered_set<position> validPositionPlaceNew(PlayerColor color){
        unordered_set<position> seen;
        unordered_set<position> valid;
        for(auto op :occupied){
            piece p=at(op)->top();
            if(p.col==color){
                for(auto possiblePosition:op.neighbor()){
                    if(seen.count(possiblePosition)) break;
                    seen.insert(possiblePosition);
                    if(isFree(possiblePosition)){
                        bool hasOtherColor=false;
                        for(auto n:possiblePosition.neighbor()){
                            if(!isFree(n) && at(n)->top().col!=color){
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


    vector<position> occupiedEdge(position pos){
        vector<position> ris;
        for(auto n:pos.neighbor()){
            if(!isFree(n)){
                ris.push_back(n);
            }
        }
        return ris;
    }

    
    bool hiveRuleWithout(piece b,int turn){
        if(getPosition(b)==NULL_POSITION)return true;
        if(at(getPosition(b))->size()>2) return true;
        if(turn!=lastUpdateTurn){
            lastUpdateTurn=turn;
            find_articulation();
        }
        return not_movable_position.count(getPosition(b));

    }


    private:
    int timer=0;
    int lastUpdateTurn=-1;
    unordered_set<position> not_movable_position;
    unordered_set<position> visited_dfs;
    unordered_map<position,int> tin,low;

    void find_articulation(){
        if(occupied.empty())return ;

        not_movable_position.clear();
        

        visited_dfs.clear();
        for(position e:occupied){
            tin[e]=-1;
            low[e]=-1;
        }
        position startPos=*(occupied.begin());
        dfs(startPos);
    }


    void dfs(position &v, position &p = NULL_POSITION) {
        visited_dfs.insert(v);
        tin[v] = low[v] = timer++;
        int children=0;
        
        for (position to : occupiedEdge(v)) {
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