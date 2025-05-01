#include "direction.h"
#include "position.h"
direction allDirections[] = {RIGHT,DOWN_RIGHT,DOWN_LEFT,LEFT,UP_LEFT,UP_RIGHT};

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
 *   Converts an integer to a direction.
 *
 *   The integer is expected to be in the range 0 to 5. The corresponding direction
 *   is returned.
 *
 *   :param n: Integer to convert.
 *   :type n: int
 *   :return: Direction.
 *   :rtype: direction
 */
direction numberToDirection(int n){
    return allDirections[n];
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
    return numberToDirection((d+3)%6);
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
pair<int,int> associatedDifference(direction d){
    return make_pair((movementCircleClockwise[d].first+SIZE_BOARD)%SIZE_BOARD,(movementCircleClockwise[d].second+SIZE_BOARD)%SIZE_BOARD);
}

/**
 *   Extracts a direction from a string, given a string representation of
 *   a move.
 *
 *   Given a string representation of a move, this function extracts the
 *   direction moved, which is the first character of the string.
 *
 *   The function checks if the first character of the string is a '-', '\\'
 *   or '/' character, and if so, returns the direction represented by the
 *   second character of the string.
 *
 *   If the first character is not one of the above, the function looks for
 *   the first occurrence of '-', '\\', or '/' in the string, and returns
 *   the direction represented by the substring from the beginning of the string
 *   to the found character.
 *
 *   If no such character is found, the function returns the direction OVER.
 *
 *   :param s: String representation of the move.
 *   :type s: string
 *   :return: Direction moved, represented as a direction object.
 *   :rtype: direction
 */
direction extractDirection(string s){
    if(s[0]=='-') return LEFT;
    if(s[0]=='/') return DOWN_LEFT;
    if(s[0]=='\\') return UP_LEFT;
    int l=s.length();
    if(s[l-1]=='-') return RIGHT;
    if(s[l-1]=='\\') return DOWN_RIGHT;
    if(s[l-1]=='/') return UP_RIGHT;
    return OVER;
}

string insertDirection(string s, direction d) {
    switch (d) {
        case RIGHT: return s+"-";
        case DOWN_RIGHT: return s+"\\";
        case DOWN_LEFT: return "/"+s;
        case LEFT: return "-"+s;
        case UP_LEFT: return "\\"+s;
        case UP_RIGHT: return s+"/";
        case OVER: return s;
        default: return "";
    }
}