#ifndef GAMEBOARD_H
#define GAMEBOARD_H

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
#include "action.h"
#include "direction.h"
using namespace  std;
#define HIGHT_BOARD 6

class gameboard{
    public:
    
    piece gb[SIZE_BOARD][SIZE_BOARD][HIGHT_BOARD];  // Vector of stacks that contain bugs. One stack at each position. The board is thus 100*100
    unordered_map<piece,position> bugPosition = unordered_map<piece,position>(); 
    unordered_set<position> occupied = unordered_set<position>();
    int high[SIZE_BOARD][SIZE_BOARD];
    //Initialization

    gameboard(){}

    //Reset

    void reset();

    //Manage positions

    //piece* at(const position &pos);
    void popPosition(const position &pos);
    bool isFree(const position &pos);
    bool isAtLevel1(const position &pos);


    //Manage pieces
    
    position getPosition(const piece &p);
    void updatePos(const piece &bug,const position &pos);
    void removePiece(const piece &b);    
    void addPiece(const action &a);
    void addPiece(const position &pos, const piece &b);
    bool isTop(const piece &bug);
    bool canPieceMove(const piece &b,int turn);
    piece topPiece(const position &pos);
    int getHight(const position &pos);

    //Main function to understand the movements
    bool canHorizontalSlide( position &from,  position &to);
    pair<piece, direction> getRelativePositionIfCanMove( position &to,  position &from, bool canOver);
    pair<piece, direction> getSlidingMoveAtLevel1(position &to, position &from);
    bool canMoveWithoutBreakingHiveRule(const piece &b,int turn);


    //And the placing
    vector<pair<piece,direction>> validPositionPlaceNew(PlayerColor color);
    vector<position> occupiedEdge(position &pos);

    
    //PRIVATE FUNCTIONS TO FIND ARTICULATION POINTS (HIVE RULE) AND CALCULATE POSITION

    private:

    int timer=0;
    int lastUpdateTurn=-1;
    unordered_set<position> not_movable_position;
    unordered_set<position> visited_dfs;
    unordered_map<position,int> disc,low;

    /**
     * \brief Calculate the position of a bug given its current position and a direction.
     *
     * Given a bug and a direction, this function returns the position that the
     * bug would be at if it were moved in the given direction. Used to calculate
     * the position of a bug after a move.
     *
     * \param b The bug to calculate the new position of.
     * \param d The direction to move the bug in.
     * \return The new position of the bug after moving in the given direction.
     */
    position calcPosition(const piece &b, direction d){
        position pos = getPosition(b);
        pos = pos.applayMove(d);
        return pos;
    }

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
    void dfs(position &v, const position &p = NULL_POSITION) {

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
#endif