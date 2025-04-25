#include "direction.h"
#include "position.h"




const int RIGHT=0;
const int DOWN_RIGHT=1;
const int DOWN_LEFT=2;
const int LEFT=3;
const int UP_LEFT=4;
const int UP_RIGHT=5;
const int OVER=6;


pair<int,int> movementCircleClockwise[]={
    make_pair(+1,0),
    make_pair(0,+1),
    make_pair(-1,+1),
    make_pair(-1,0),
    make_pair(0,-1),
    make_pair(+1,-1),
    make_pair(0,0)
};

/**
 *   Converts a direction to a string representation of a movement.
 *
 *   :param name: Short name of the bug piece.
 *   :type name: string
 *   :param dir: Direction of the destination tile with respect to the relative bug piece.
 *   :type dir: direction
 *   :return: String representation of the movement.
 *   :rtype: string
 */
string nameDirToString(string name, direction dir){
    switch (dir) {
        case RIGHT: return name+"-";
        case DOWN_RIGHT: return name+"\\";
        case DOWN_LEFT: return "/"+name;
        case LEFT: return "-"+name;
        case UP_LEFT: return "\\"+name;
        case UP_RIGHT: return name+"/";
        case OVER: return name;
        default: return "";
    }
}



/**
 *   Returns the opposite direction.
 *
 *   Given a direction, this function returns the direction that is 180 degrees
 *   opposite to the given direction.
 *
 *   :param d: Direction to find the opposite of.
 *   :type d: direction
 *   :return: Opposite direction.
 *   :rtype: direction
 */
direction oppositeDir(direction d){
    return ((d+3)%6);
}

/**
 *   Returns the pair of integers associated with a direction.
 *
 *   This function takes a direction as a parameter and returns a pair of integers
 *   that represent the movement delta associated with that direction.
 *
 *   :param d: Direction to get the associated difference of.
 *   :type d: direction
 *   :return: The pair of integers associated with the direction.
 *   :rtype: pair<int,int>
 */



pair<unsigned int,unsigned int> associatedDifferenceV[]={
    make_pair(+1,0),
    make_pair(0,+1),
    make_pair(31,+1),
    make_pair(31,0),
    make_pair(0,31),
    make_pair(+1,31),
    make_pair(0,0)
};
pair<unsigned int,unsigned int> associatedDifference(direction d){
    
    return associatedDifferenceV[d];
}

