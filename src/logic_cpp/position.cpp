#include <string>
#include <vector>
#include "position.h"
using namespace std;

/*
USING THIS GRID:


 |       |       |
 |  0,-1 | +1,-1 |
 |   UL  |   UR  |
  \     / \     /
   \   /   \   /
    \ /     \ /
     |       |
-1,0 |  0,0  | +1,0
 L   |       |   R
    / \     / \
   /   \   /   \
  /     \ /     \
 |       |       |
 | -1,+1 |  0,+1 |
    DL       DR


*/


/**
 * \brief Creates a position with the given coordinates.
 *
 * \param x The x-coordinate of the position.
 * \param y The y-coordinate of the position.
 */
position::position(int x,int y){
    first=x;
    second=y;
}


/**
 * \brief Copy constructor.
 *
 * \param p The position to be copied.
 */

position::position(const position &p){
    first=p.first;
    second=p.second;
}
    
/**
 * \brief Default constructor. TODO: change it to another invalid position
 *
 * Initializes the position with very large values, so that any valid position
 * is smaller.
 */
position::position(){
    first=10000;
    second=10000;
}

/**
 * \brief Returns the neighboring positions in a hexagonal grid.
 *
 * Calculates and returns a vector of positions that are adjacent to
 * the current position in all six possible directions of a hexagonal grid.
 *
 * \return A vector containing the six neighboring positions.
 */
vector<position> position::neighbor(){
    vector<position> v;
    v.emplace_back(first,second+1);
    v.emplace_back(first+1,second);
    v.emplace_back(first+1,second-1);
    v.emplace_back(first,second-1);
    v.emplace_back(first-1,second);
    v.emplace_back(first-1,second+1);
    return v;       
}


/**
 * \brief Applies a movement in the specified direction.
 *
 * Adjusts the current position by applying the movement delta
 * associated with the given direction, resulting in a new position.
 *
 * \param d The direction in which to move.
 * \return A new position resulting from the movement.
 */
position position::applayMove(direction d){
    pair<int,int> delta = associatedDifference(d);
    position p(first+delta.first,second+delta.second);
    return p;
}



/**
 * \brief Returns the direction of movement from one position to another.
 *
 * Given two positions, calculates and returns the direction of movement
 * that would be needed to move from the first position to the second.
 *
 * \param from The starting position.
 * \param to The destination position.
 * \return The direction of movement from `from` to `to`.
 */
direction getMovementDirection(position from, position to){
    pair<int,int> diff(to.first-from.first,to.second-from.second);

    for (int i = 0; i<6; i++){
        pair<int,int> p = movementCircleClockwise[i]; 
        if(p.first==diff.first && p.second==diff.second){
            return numberToDirection(i);
        }
    }
    return INVALID;
}



/**
 * \brief Checks if two positions are equal.
 *
 * Compares two positions for equality, returning true if both
 * the x and y coordinates are equal, and false otherwise.
 *
 * \param p1 The first position to compare.
 * \param p2 The second position to compare.
 * \return true if the positions are equal, false otherwise.
 */
bool operator==(const position &p1,const position& p2){
    return (p1.first==p2.first && p1.second==p2.second);
}

/**
 * \brief Checks if two positions are not equal.
 *
 * Compares two positions for inequality, returning true if either
 * the x or y coordinates are not equal, and false otherwise.
 *
 * \param p1 The first position to compare.
 * \param p2 The second position to compare.
 * \return true if the positions are not equal, false otherwise.
 */
bool operator!=(const position &p1,const position& p2){
    return !(p1==p2);
}

/**
 * \brief Checks if two positions are next to each other.
 *
 * Compares two positions and returns true if they are next to each other
 * in any of the six possible directions of a hexagonal grid, and false
 * otherwise.
 *
 * \param p1 The first position to compare.
 * \param p2 The second position to compare.
 * \return true if the positions are next to each other, false otherwise.
 */
bool isNear(const position &p1, const position &p2){
    if(getMovementDirection(p1,p2)!=INVALID) 
        return true;
    return false;
}



/**
 * \brief Determines the neighboring positions next to both input positions.
 *
 * Given two positions, calculates and returns the two positions that are adjacent
 * to the first position and lie to the sides of the direction toward the second position
 * on a hexagonal grid. The function effectively finds the two positions that form a 
 * triangle with the given two positions.
 *
 * \param p1 The first position.
 * \param p2 The second position.
 * \return A vector containing the two neighboring positions of `p1` that are adjacent 
 *         to the line formed by `p1` and `p2`.
 */

vector<position> nearBoth(position &p1,position &p2){
    direction dir = getMovementDirection(p1,p2);
    int n=dir;
    int m1=(n+1)%6;
    int m2=(n+5)%6;
    vector<position> ris;
    ris.push_back(p1.applayMove(numberToDirection(m1)));
    ris.push_back(p1.applayMove(numberToDirection(m2)));
    return ris;
}



