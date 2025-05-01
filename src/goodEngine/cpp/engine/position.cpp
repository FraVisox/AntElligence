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

//INVALID POSITION
position NULL_POSITION(10000, 10000);


/**
 * \brief Constructs a position from coordinates.
 *
 * \param x The x coordinate of the position.
 * \param y The y coordinate of the position.
 */
position::position(int x,int y){
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
    
    v.emplace_back((first  +SIZE_BOARD)%SIZE_BOARD,(second+1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first+1+SIZE_BOARD)%SIZE_BOARD,(second  +SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first+1+SIZE_BOARD)%SIZE_BOARD,(second-1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first  +SIZE_BOARD)%SIZE_BOARD,(second-1+SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first-1+SIZE_BOARD)%SIZE_BOARD,(second  +SIZE_BOARD)%SIZE_BOARD);
    v.emplace_back((first-1+SIZE_BOARD)%SIZE_BOARD,(second+1+SIZE_BOARD)%SIZE_BOARD);
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
position position::applayMove(direction d){
    pair<int,int> delta = associatedDifference(d);
    position p((first+delta.first+SIZE_BOARD)%SIZE_BOARD,(second+delta.second+SIZE_BOARD)%SIZE_BOARD);
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
direction getMovementDirection(const position &from, const position &to){
    pair<int,int> diff((to.first-from.first+SIZE_BOARD)%SIZE_BOARD,(to.second-from.second+SIZE_BOARD)%SIZE_BOARD);

    for (int i = 0; i<6; i++){
        pair<int,int> p = movementCircleClockwise[i]; 
        if((p.first+SIZE_BOARD)%SIZE_BOARD==(diff.first+SIZE_BOARD)%SIZE_BOARD && (p.second+SIZE_BOARD)%SIZE_BOARD==(diff.second+SIZE_BOARD)%SIZE_BOARD){
            return numberToDirection(i);
        }
    }
    return INVALID;
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
    return ((p1.first+SIZE_BOARD)%SIZE_BOARD==(p2.first+SIZE_BOARD)%SIZE_BOARD && (p1.second+SIZE_BOARD)%SIZE_BOARD==(p2.second+SIZE_BOARD)%SIZE_BOARD);
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

bool isNear(const position &p1, const position &p2){
    if(getMovementDirection(p1,p2)!=INVALID) 
        return true;
    return false;
}

/**
 * \brief Computes the two positions between which p1 and p2 are located.
 *
 * Given two positions p1 and p2, this function returns the two positions
 * between which p1 and p2 are located. The positions are computed by looking
 * for the two directions that are adjacent to the direction between p1 and
 * p2.
 *
 * \param p1 The first position.
 * \param p2 The second position.
 * \return A vector containing the two positions between p1 and p2.
 */
vector<position> nearBoth(position &p1, position &p2){
    direction dir = getMovementDirection(p1,p2);
    int n=dir;
    int m1=(n+1)%6;
    int m2=(n+5)%6;
    vector<position> ris;
    ris.push_back(p1.applayMove(numberToDirection(m1)));
    ris.push_back(p1.applayMove(numberToDirection(m2)));
    return ris;
}



