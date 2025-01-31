#include "action.h"
#include <sstream>
#include <iostream>

/**
 * \brief Constructs a movement action given a piece to move, another piece to which the moved piece is relative, and a direction.
 *
 * Given a piece and another piece with respect to which the bug is moved, and a direction, this function returns an action
 * object which represents a movement action. The action object contains the piece to move, the other bug piece, the direction
 * of movement relative to the other bug piece, and the type of action, which is a MOVEMENT action.
 *
 * \param p The piece to move.
 * \param other The other bug piece with respect to which the moved piece is moved.
 * \param dir The direction of movement relative to the other bug piece.
 * \return An action object representing a movement action.
 */
action movement(piece p, piece other, direction dir){
    action a;
    a.bug=p;

    a.otherBug = other;
    a.relativeDir = dir;
    a.actType=MOVEMENT;
    return a;
}

/**
 * \brief Constructs a place action.
 *
 * Creates an action object representing the placement of a piece at a specified position.
 * The action object includes the piece to be placed, the target position, and indicates
 * that this action is a PLACE action.
 *
 * \param p The piece to place.
 * \param d The destination position.
 * \return An action object representing a place action.
 */
action placePiece(piece p, piece other, direction d){
    action a;
    a.bug=p;
    a.otherBug = other;
    a.relativeDir = d;
    a.actType=PLACE;
    return a;
}

/**
 * \brief Constructs a place first action.
 *
 * Creates an action object representing the placement of a piece on the
 * starting position. The action object includes the piece to be placed and
 * indicates that this action is a PLACEFIRST action.
 *
 * \param p The piece to place.
 * \return An action object representing a place first action.
 */
action placeFirst(piece p){
    action a(p);
    a.actType = PLACEFIRST;
    return a;
}

/**
 * \brief Constructs a pass action.
 *
 * Creates an action object representing a pass action, which allows the player
 * to end their turn without performing any other action. The action object does
 * not contain any information other than the type of action, which is a PASS
 * action.
 *
 * \return An action object representing a pass action.
 */
action pass(){
    action a;
    a.actType = PASS;
    return a;
}


/**
 * \brief Converts a string representation of an action to an action object.
 *
 * Parses a string containing the details of an action, including the bug piece
 * to be moved and its relative position and direction, and constructs an
 * action object representing this move. The string is expected to have two
 * parts separated by a space: the first part representing the bug piece, and
 * the second part representing the relative position and direction.
 *
 * \param s The string representation of the action.
 * \return An action object constructed from the string details.
 */

action parseAction(string s){
    if (s == "pass") {
        return pass();
    }
    int end = s.find(' ');
    if ( (long long unsigned int) end == string::npos) {
        return placeFirst(piece(s));
    }
    string first = s.substr(0, end);
    string second = s.substr(end + 1);
    action a = movement(piece(first), piece(second), extractDirection(second));
    return a;
}



/**
 * \brief Converts an action object representing a movement action to a string.
 *
 * Given an action object, this function converts its details to a string
 * representation. The string is a concatenation of the bug piece to be moved
 * and its relative position and direction. The relative position and direction
 * are represented by a single string, which is the concatenation of the
 * relative position bug piece and the direction indicator.
 *
 * \param a The action object to be converted to a string.
 * \return A string representation of the action object.
 */
string MovementToString(action a){
    stringstream ss;
    ss << a.bug.toString();
    ss << " ";
    ss << nameDirToString(a.otherBug.toString(), a.relativeDir);
    return ss.str();
}


string ActionToString(action a){
    switch (a.actType)
    {
        case PLACE:
        case MOVEMENT:
            return MovementToString(a);

        case PLACEFIRST:
            return a.bug.toString();
        
        case PASS:
            return "pass";
    }
    return "";
}