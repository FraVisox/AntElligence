#include "action.h"
#include <sstream>

//Invalid action (it has an invalid piece)
action INVALID_ACTION = action(INVALID_PIECE);

/**
 *   Constructs a movement action.
 * 
 *   :param p: Bug piece moved.
 *   :type p: piece
 *   :param other: Bug piece relative to which the other bug piece is moved.
 *   :type other: piece
 *   :param dir: Direction of the destination tile with respect to the relative bug piece.
 *   :type dir: direction
 *   :return: Action representing the movement.
 *   :rtype: action
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
 *   Constructs a place action.
 * 
 *   :param p: Bug piece to be placed.
 *   :type p: piece
 *   :param other: Bug piece relative to which the new piece is placed.
 *   :type other: piece
 *   :param d: Direction of the destination tile with respect to the relative bug piece.
 *   :type d: direction
 *   :return: Action representing the placement.
 *   :rtype: action
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
 *   Constructs a place first action.
 * 
 *   :param p: Bug piece to be placed.
 *   :type p: piece
 *   :return: Action representing the placement of the first bug.
 *   :rtype: action
 */
action placeFirst(piece p){
    action a(p);
    a.actType = PLACEFIRST;
    return a;
}

/**
 *   Constructs a pass action.
 * 
 *   :return: Action representing the pass of the player's turn.
 *   :rtype: action
 */
action pass(){
    action a;
    a.actType = PASS;
    return a;
}


/**
 *   Parses an action string.
 * 
 *   :param s: Action string.
 *   :type s: string
 *   :param inHandPiece: Set of pieces in hand.
 *   :type inHandPiece: unordered_set<piece>
 *   :raises ValueError: If the action string is not valid.
 *   :return: Action parsed from the string.
 *   :rtype: action
 */
action parseAction(string s, const unordered_set<piece>& inHandPiece){
    if (s == "pass") {
        return pass();
    }
    int end = s.find(' ');
    if ( (long long unsigned int) end == string::npos) {
        return placeFirst(piece(s));
    }
    string first = s.substr(0, end);
    string second = s.substr(end + 1);
    if (inHandPiece.find(piece(first)) != inHandPiece.end()) {
        return placePiece(piece(first), extractPiece(second), extractDirection(second));
    }
    return movement(piece(first), extractPiece(second), extractDirection(second));
}


/**
 *   Converts an action to a string representation of a movement.
 * 
 *   :param a: Action representing a movement.
 *   :type a: action
 *   :return: String representation of the movement.
 *   :rtype: string
 */
string MovementToString(const action &a){
    stringstream ss;
    ss << a.bug.toString();
    ss << " ";
    ss << nameDirToString(a.otherBug.toString(), a.relativeDir);
    return ss.str();
}


/**
 *   Converts an action to a string representation of the action.
 * 
 *   :param a: Action to be converted.
 *   :type a: action
 *   :return: String representation of the action.
 *   :rtype: string
 */
string ActionToString(const action &a){
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