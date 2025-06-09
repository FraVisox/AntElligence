#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#define TOTAL_POSSIBLE_MOVES 1575
#include <vector>
#include <bitset>
#include "piece.h"
#include "position.h"
#include "enums.h"
#include "direction.h"
using namespace  std;
#define HIGHT_BOARD 6

class gameboard{
    public:
    gameboard();
    gameboard(GameType);

    pieceT gb[8][1024];  // Vector of stacks that contain bugs. One stack at each position. The board is thus 100*100
    positionT bugPosition[32];
    bitset<1024> occupied;
    int high[1024];
    
    bitset<32> isPlaced;

        /*
    *   Valid moves are the followin in this order:
    *   X (0) -> 1 pass move
    *   X (1) -> 14  moves for place the peice
    *   X (15) -> 672 6*8*14 place
    *   X (687) -> 6   positions for Q
    *   X (693) -> 6*2   positions for B1,B2
    *   X (705) -> 6*3   positions for G1,G2,G3
    *   X (723) -> 504=28*6*3 for A1,A2,A3
    *   X (1227) -> 72=36*2  moves for S1,S2
    *   X (1299) -> 6  for P moves
    *   X (1305) -> 30 for P ability
    *   X (1335) -> 36  for L
    *   X (1371) -> 168 mosquito copy all pieces
    *   x (1539) -> 6 mosquito copy beetle and going over
    *   x (1545) -> 30 mosquito copy pillbug
    *   Total 
    */  // Idea in background
    uint8_t isValidMoveBitmask[1575];
    uint8_t _isValidMoveBitmask_rel_pos[1575];

    //Initialization
    //bool checIfCanMove(position &to, position &from, bool canOver);

    
    //And the placing
    positionT validPositionPlaceBuffer[400];
    int numValidPosition;
    void computeValidPositionPlaceNew(PlayerColor color);

    

    //Reset
    void copy(gameboard& g);

    //Manage positions

    void popPosition(const positionT &pos);
    bool isFree(const positionT &pos);
    bool isAtLevel1(const positionT &pos);
    bitset<308> toHash();
    //Manage pieces
    
    positionT getPosition(const pieceT &p);
    void updatePos(const pieceT &bug,const positionT &pos);
    void removePiece(const pieceT &b);    
    void addPiece(const positionT &pos, const pieceT &b);
    bool isTop(const pieceT &bug);
    bool canPieceMove(const pieceT &b,int turn);
    pieceT topPiece(const positionT &pos);
    pieceT basePiece(const positionT &pos);
    int getHight(const positionT &pos);

    //Main function to understand the movements
    bool canMoveWithoutBreakingHiveRule(const pieceT &b,int turn);

    bool canSlideToFreeDir(const positionT &from,const positionT &to,const direction n);
  
    //PRIVATE FUNCTIONS TO FIND ARTICULATION POINTS (HIVE RULE) AND CALCULATE POSITION

    private:

    int timer=0;
    int lastUpdateTurn=-1;
    bitset<SIZE_BOARD*SIZE_BOARD> not_movable_position;
    bitset<SIZE_BOARD*SIZE_BOARD> visited_dfs;
    int disc[SIZE_BOARD*SIZE_BOARD],low[SIZE_BOARD*SIZE_BOARD];


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

        positionT startPos;
        for(pieceT p=0;p<32;p++){
            if(isPlaced[p]){
                positionT e=bugPosition[p];
                startPos=e;
                disc[e]=-1; //discovered time
                low[e]=-1; //lowest discovered time
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
    void dfs(positionT &v, const positionT &p = NULL_POSITION) {

        //Mark this as visited
        visited_dfs.set(v,1);

        //Set the discovery time and low time as current time
        disc[v] = low[v] = timer++;

        int children=0;
        for (int dir=0;dir<6;dir++){
            positionT to=applayMove(v,dir);
            if(isFree(to))continue;
            //If this is a back edge, update low
            if (visited_dfs[to]) {
                low[v] = min(low[v], disc[to]);
            } else {
                //Make it a children of this node and visit it
                dfs(to, v);
                //Check if the subtree rooted at v has a connection to one of the ancestors of u
                low[v] = min(low[v], low[to]);
                
                //If the subtree rooted at to has not a connection to one of the ancestors of u, this is an articulation point
                if (low[to] >= disc[v] && p!=NULL_POSITION)
                    not_movable_position.set(v,1);
                
                ++children;
            }
        }
        if(p == NULL_POSITION && children > 1)
            not_movable_position.set(v,1);
    }

};

#endif