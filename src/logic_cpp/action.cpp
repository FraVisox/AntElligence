#include "position.h"
#include "piece.h"

using namespace std;

enum TypeAction{
    MOVEMENT,
    PLACE,
    PLACEFIRST,
    PASS
};

class action{
    public:
    piece bug;
    position pos;
    TypeAction actType;

    piece otherBug;
    Direction relativeDir;

    action(piece p,position d):bug(p),pos(d){}
    action(piece p):bug(p){}
    action(){}
};

/**
 * \brief Constructs a movement action.
 *
 * Given a piece and a destination position, this function returns an action
 * object which represents a movement action. The action object contains the
 * piece to move, the destination position, and the type of action, which is a
 * MOVEMENT action.
 *
 * \param p The piece to move.
 * \param d The destination position.
 * \return An action object representing a movement action.
 */
action movement(piece p,position d){
    action a;
    a.bug=p;
    a.pos=d;
    a.actType=MOVEMENT;
    return a;
}


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
action movement(piece p, piece other, Direction dir){
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
action placePiece(piece p, position d){
    action a(p,d);
    a.actType = PLACE;
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
    a.actType = PASS; //TODO: does it work?
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

action StringToAction(string s){
    int end = s.find(' ');
    string first = s.substr(0, end);
    string second = s.substr(end + 1);
    action a = movement(StringToBug(first), StringToBug(second), ExtractDirection(second));
    return a;
}