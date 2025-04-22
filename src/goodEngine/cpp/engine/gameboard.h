#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <set>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <bitset>
#include "piece.h"
#include "position.h"
#include "enums.h"
#include "action.h"
#include "direction.h"
using namespace  std;
#define HIGHT_BOARD 6

class gameboard{
    public:
    gameboard();
    pieceT gb[SIZE_BOARD][SIZE_BOARD][HIGHT_BOARD];  // Vector of stacks that contain bugs. One stack at each position. The board is thus 100*100
    position bugPosition[32];
    bitset<SIZE_BOARD*SIZE_BOARD> occupied;
    int high[SIZE_BOARD][SIZE_BOARD];
    
    bitset<32> isPlaced;
    //Initialization
    

    
    //And the placing
    vector<pair<pieceT,direction>> validPositionPlaceNew(PlayerColor color);
    vector<position> occupiedEdge(position &pos);

    

    //Reset
    void copy(gameboard& g);

    //Manage positions

    void popPosition(const position &pos);
    bool isFree(const position &pos);
    bool isAtLevel1(const position &pos);


    //Manage pieces
    
    position getPosition(const pieceT &p);
    void updatePos(const pieceT &bug,const position &pos);
    void removePiece(const pieceT &b);    
    //void addPiece(const action &a);
    void addPiece(const position &pos, const pieceT &b);
    bool isTop(const pieceT &bug);
    bool canPieceMove(const pieceT &b,int turn);
    pieceT topPiece(const position &pos);
    int getHight(const position &pos);

    //Main function to understand the movements
    bool canHorizontalSlide( position &from,  position &to);
    pair<pieceT, direction> getRelativePositionIfCanMove( position &to,  position &from, bool canOver);
    pair<pieceT, direction> getSlidingMoveAtLevel1(position &to, position &from);
    bool canMoveWithoutBreakingHiveRule(const pieceT &b,int turn);


    
    //PRIVATE FUNCTIONS TO FIND ARTICULATION POINTS (HIVE RULE) AND CALCULATE POSITION

    private:

    int timer=0;
    int lastUpdateTurn=-1;
    bitset<SIZE_BOARD*SIZE_BOARD> not_movable_position;
    bitset<SIZE_BOARD*SIZE_BOARD> visited_dfs;
    int disc[SIZE_BOARD*SIZE_BOARD],low[SIZE_BOARD*SIZE_BOARD];

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
    position calcPosition(const pieceT &b, direction d){
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
        if(occupied.none())
            return;

        not_movable_position.reset();
        visited_dfs.reset();

        position startPos;
        for(pieceT p=0;p<32;p++){
            if(isPlaced[p]){
                position e=bugPosition[p];
                startPos=e;
                disc[e.toInt()]=-1; //discovered time
                low[e.toInt()]=-1; //lowest discovered time
            }
        }

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
        visited_dfs.set(v.toInt(),1);

        //Set the discovery time and low time as current time
        disc[v.toInt()] = low[v.toInt()] = timer++;

        int children=0;
        for (position to : occupiedEdge(v)) {
            //If this is a back edge, update low
            if (visited_dfs[to.toInt()]) {
                low[v.toInt()] = min(low[v.toInt()], disc[to.toInt()]);
            } else {
                //Make it a children of this node and visit it
                dfs(to, v);
                //Check if the subtree rooted at v has a connection to one of the ancestors of u
                low[v.toInt()] = min(low[v.toInt()], low[to.toInt()]);
                
                //If the subtree rooted at to has not a connection to one of the ancestors of u, this is an articulation point
                if (low[to.toInt()] >= disc[v.toInt()] && p!=NULL_POSITION)
                    not_movable_position.set(v.toInt(),1);
                
                ++children;
            }
        }
        if(p == NULL_POSITION && children > 1)
            not_movable_position.set(v.toInt(),1);
    }

};
#endif