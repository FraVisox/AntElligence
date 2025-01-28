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


    stack<piece> gb[100][100];  // Vector of stacks that contain bugs. One stack at each position. The board is thus 100*100
    unordered_map<piece,position> bugPosition; 
    unordered_set<position> occupied;

    gameboard(){}

    /**
     * \brief Get the stack of pieces at a given position.
     *
     * \param pos The position to get the stack of pieces from.
     * \return A pointer to the stack of pieces at pos.
     */
    stack<piece>* at(position pos){
        return &gb[(100+(pos.first%100))%100][(100+(pos.second%100))%100];
    }
    
    /**
     * \brief Get the position of a bug on the board.
     *
     * \param p The bug to get the position of.
     * \return The position of bug p.
     */
    position getPosition(piece p){
        return bugPosition.at(p);
    }
    
    /**
     * \brief Update the position of a bug on the board.
     *
     * \param bug The bug to update the position of.
     * \param pos The new position of bug.
     */
    void updatePos(piece bug, const position &pos){
        bugPosition.insert_or_assign(bug,pos);
    }


    /**
     * \brief Remove the top piece from a given position and update the
     *        occupied set.
     *
     * If the position is not free, remove the top piece from the position
     * and update the position of the removed piece to NULL_POSITION.
     * Then, if the position is free after popping the top piece, remove
     * the position from the occupied set.
     *
     * \param pos The position to pop the top piece from.
     */
    void popPosition(position pos){
        if(!isFree(pos)){
            updatePos(at(pos)->top(),NULL_POSITION);
            at(pos)->pop();
        }
        if(isFree(pos))
            occupied.extract(pos);
    }


    /**
     * \brief Remove a bug from the board.
     *
     * If the bug is the top piece at its position, remove it from the board.
     * This involves popping the piece from its position and removing its
     * position from the occupied set. Additionally, remove the bug from
     * the bugPosition map.
     *
     * \param b The bug to remove from the board.
     */
    void removePiece(piece b){
        if(isTop(b)){
            popPosition(getPosition(b));
            bugPosition.extract(b);
        }
    }

    /**
     * \brief Add a piece to the board.
     *
     * Pushes the given bug onto the given position and updates the bug's
     * position in the bugPosition map. Additionally, inserts the position
     * into the occupied set.
     *
     * \param pos The position to add the piece to.
     * \param b The bug to add to the board.
     */
    void addPiece(position pos,piece b){
        at(pos)->push(b);
        updatePos(b,pos);
        occupied.insert(pos);
    }

    /**
     * \brief Check if a position is free of pieces.
     *
     * \param pos The position to check.
     * \return true if the position is free, false otherwise.
     */
    bool isFree(position &pos){
        return at(pos)->empty();
    }

    /**
     * \brief Check if the given bug is the top piece at its position.
     *
     * \param bug The bug to check.
     * \return true if the bug is the top piece at its position, false otherwise.
     */
    bool isTop(piece bug){
        position pos=getPosition(bug);
        return (at(pos)->top()==bug);
    }

    /**
     * \brief Check if the given bug can move on the given turn.
     *
     * Checks if the given bug is the top piece at its position and if the
     * hive rule is satisfied for the given bug on the given turn.
     *
     * \param b The bug to check.
     * \param turn The turn to check.
     * \return true if the bug can move on the given turn, false otherwise.
     */
    bool canPieceMove(piece b,int turn){
        return (isTop(b) && canMoveWithoutBreakingHiveRule(b,turn));
    }

    /**
     * \brief Check if a bug can slide to the given position.
     *
     * A bug can slide to the given position if the given position is
     * free, the bug is at a position adjacent to the given position, and the
     * two positions on the path between the bug and the given position are
     * both free.
     *
     * \param from The position of the bug.
     * \param to The position the bug would like to slide to.
     * \return true if the bug can slide to the given position for free, false otherwise.
     */
    bool canSlideFree(position from, position to){
        if(isFree(to) && isNear(to, from)){
            vector<position> v= nearBoth(from,to);
            position p1=v[0], p2=v[1];
            if(isFree(p1) && isFree(p2)) 
                return true;
        }
        return false;
    }

    /**
     * \brief Get the top piece at a given position.
     *
     * \param pos The position to get the top piece from.
     * \return The top piece at the given position.
     *
     * \throws std::string if the given position is empty.
     */
    piece topPiece(position pos){
        if(!isFree(pos)){
            return at(pos)->top();
        }
        throw "Asked piece for empty pos";
    }

    
    /**
     * \brief Get all valid positions to place a new piece.
     *
     * A position is valid if it is adjacent to a piece of the given color, and
     * if all the neighbors of the position are either free or of the same color.
     *
     * \param color The color of the player for which to get the valid positions.
     * \return An unordered set of all valid positions to place a new piece.
     */
    unordered_set<position> validPositionPlaceNew(PlayerColor color){
        unordered_set<position> seen;
        unordered_set<position> valid;
        for(auto op : occupied){
            piece p = at(op)->top();
            if(p.col == color){
                for(auto possiblePosition:op.neighbor()){
                    if(seen.count(possiblePosition)) 
                        continue;

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


    /**
     * \brief Get the occupied neighboring positions.
     *
     * This function returns a vector of positions that are neighbors
     * to the given position and are currently occupied by pieces.
     *
     * \param pos The position to check for occupied neighbors.
     * \return A vector containing the positions of all occupied neighbors.
     */

    vector<position> occupiedEdge(position pos){
        vector<position> ris;
        for(auto n:pos.neighbor()){
            if(!isFree(n)){
                ris.push_back(n);
            }
        }
        return ris;
    }

    bool isAtLevel1(position pos){
        return at(pos)->size()==1;
    }
    
    /**
     * \brief Check if the bug can be moved.
     *
     * A bug can be moved if it is possible to move it without breaking
     * the hive rule.
     *
     * \param b The bug to check.
     * \param turn The current turn.
     * \return true if the bug is part of the hive, false otherwise.
     */
    bool canMoveWithoutBreakingHiveRule(piece b,int turn){
        if(getPosition(b)==NULL_POSITION)
            return true;
        //If there is a bug under this one
        if(at(getPosition(b))->size()>2) 
            return true;
        //Update cache
        if(turn != lastUpdateTurn){
            lastUpdateTurn = turn;
            find_articulation();
        }
        return not_movable_position.count(getPosition(b));
    }


    private:
    int timer=0;
    int lastUpdateTurn=-1;
    unordered_set<position> not_movable_position;
    unordered_set<position> visited_dfs;
    unordered_map<position,int> disc,low;

    /**
     * \brief Find the articulation points of the occupied positions.
     *
     * Runs a depth-first search to find the articulation points of the
     * occupied positions. The articulation points are the positions that,
     * if removed, increase the number of connected components.
     * The articulation points are used to check if a
     * bug can be moved without breaking the hive rule.
     */
    void find_articulation(){
        if(occupied.empty())
            return;

        not_movable_position.clear();
        visited_dfs.clear();

        for(position e: occupied){
            disc[e]=-1; //discovered time
            low[e]=-1; //lowest discovered time
        }

        position startPos=*(occupied.begin());
        dfs(startPos);
    }


    /**
     * \brief Depth-first search to find articulation points.
     *
     * Runs a depth-first search on the graph of occupied positions to find
     * the articulation points.
     *
     * \param v The current position.
     * \param p The parent position, defaults to NULL_POSITION.
     */
    void dfs(position &v, position &p = NULL_POSITION) {

        //Mark this as visited
        visited_dfs.insert(v);

        //Set the discovery time and low time as current time
        disc[v] = low[v] = timer++;

        int children=0;
        for (position to : occupiedEdge(v)) {
            //If this is a back edge, update low
            if (visited_dfs.count(to)) {
                low[v] = min(low[v], disc[to]);
            } else {
                //Make it a children of this node and visit it
                dfs(to, v);
                //Check if the subtree rooted at v has a connection to one of the ancestors of u
                low[v] = min(low[v], low[to]);
                
                //If the subtree rooted at to has not a connection to one of the ancestors of u, this is an articulation point
                if (low[to] >= disc[v] && p!=NULL_POSITION)
                    not_movable_position.insert(v);
                
                ++children;
            }
        }
        if(p == NULL_POSITION && children > 1)
            not_movable_position.insert(v);
    }

};