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





/**
 * \brief Applies a movement to the current position.
 *
 * This method applies a movement in the direction specified by the
 * parameter to the current position and returns the resulting position.
 *
 * \param d The direction of the movement.
 * \return The resulting position of the movement.
 */
positionT applayMove(positionT pos,direction d) {
    return ((pos+dirDif[d])&1023);
}






