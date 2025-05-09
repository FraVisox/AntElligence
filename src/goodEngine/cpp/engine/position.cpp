#include <iostream>

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

//INVALID POSITION
position NULL_POSITION(10000, 10000);


/**
 * \brief Constructs a position from coordinates.
 *
 * \param x The x coordinate of the position.
 * \param y The y coordinate of the position.
 */
position::position(unsigned int x,unsigned int y){
    first=x;
    second=y;
}


/**
 * \brief Copy constructor for the position class.
 *
 * Initializes a new position object by copying the coordinates
 * from the provided position instance.
 *
 * \param p The position instance to copy the coordinates from.
 */
position::position(const position &p){
    first=p.first;
    second=p.second;
}
    
/**
 * \brief Constructs a position with coordinates set to a invalid value.
 *
 * This is used to mark a position as "null" or "invalid".
 */
position::position(){
    first=10000;
    second=10000;
}

/**
 * \brief Gets all the positions adjacent to the current one.
 *
 * This method returns a vector of all the positions that are
 * adjacent to the current position, in the following order:
 * up, right, down-right, down, left, up-left.
 *
 * \return A vector of all the adjacent positions.
 */
vector<position> position::neighbor(){
    vector<position> v;
    for(direction dir=0;dir<6;dir++){
        v.push_back(applayMove(dir));
    }
    /*
    v.emplace_back((first  +SIZE_BOARD)%SIZE_BOARD,(second+1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first+1+SIZE_BOARD)%SIZE_BOARD,(second  +SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first+1+SIZE_BOARD)%SIZE_BOARD,(second-1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first  +SIZE_BOARD)%SIZE_BOARD,(second-1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first-1+SIZE_BOARD)%SIZE_BOARD,(second  +SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first-1+SIZE_BOARD)%SIZE_BOARD,(second+1+SIZE_BOARD)%SIZE_BOARD);
    */
    return v;       
}

/**
 * \brief Applies a movement to the current position.
 *
 * This method applies a movement in the direction specified by the
 * parameter to the current position and returns the resulting position.
 *
 * \param d The direction of the movement.
 * \return The resulting position of the movement.
 */
position position::applayMove(direction d)const {
    position p((first+adF[d])&31,(second+adS[d])&31);
    return p;
}


/**
 * \brief Determines the movement direction from one position to another.
 *
 * This function calculates the direction needed to move from the 'from' position
 * to the 'to' position on a hexagonal grid. It computes the difference between 
 * the coordinates of the two positions and compares it to predefined movement 
 * deltas in a clockwise order to find the matching direction.
 *
 * \param from The starting position.
 * \param to The destination position.
 * \return The direction from 'from' to 'to'. Returns INVALID if no valid direction is found.
 */

const int r[]={6,1,6,4,0,6,6,5,6,6,6,6,3,2};
direction getMovementDirection(const position &from, const position &to){
    unsigned int f=(to.first-from.first)&3;
    unsigned int s=(to.second-from.second)&3;
    unsigned int ris=f*4+s;
    return r[ris];
}

/**
 * \brief Equality operator for the position class.
 *
 * Compares two position objects for equality by checking if their
 * x and y coordinates are equal.
 *
 * \param p1 The first position to compare.
 * \param p2 The second position to compare.
 * \return True if both positions have the same coordinates, false otherwise.
 */
bool operator==(const position &p1,const position& p2){
    if(p1.first==10000 && p2.first==10000){
        return true;
    }
    if(p1.first==10000 || p2.first==10000)
        return false;
    
        
    return ((p1.first&31)==(p2.first&31) && (p1.second%31)==(p2.second&31));
}

/**
 * \brief Inequality operator for the position class.
 *
 * Compares two position objects for inequality by comparing them
 * with the equality operator and inverting the result.
 *
 * \param p1 The first position to compare.
 * \param p2 The second position to compare.
 * \return True if both positions have different coordinates, false otherwise.
 */
bool operator!=(const position &p1,const position& p2){
    return !(p1==p2);
}

bool operator<(const position &p1,const position& p2){
    if(p1.first!=p2.first) return p1.first<p2.first;
    return p1.second<p2.second;
}   

/**
 * \brief Checks if two positions are adjacent.
 *
 * This function determines if two positions on a hexagonal grid are near each other
 * by checking if a valid movement direction exists between them.
 *
 * \param p1 The first position.
 * \param p2 The second position.
 * \return True if the positions are adjacent, false otherwise.
 */


position::position(int k){
    first=k&31;
    second=k>>5;
}

unsigned int position::toInt() const{
    return first+(second<<5);
}



