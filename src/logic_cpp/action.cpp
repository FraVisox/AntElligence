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
