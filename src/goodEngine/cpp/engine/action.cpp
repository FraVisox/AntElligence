#include "action.h"

//Invalid action (it has an invalid piece)

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
actionT movement(const pieceT p,const positionT &destPos){
    return (actionT)(p+((destPos&1023)*32));

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
actionT placePiece(pieceT p,positionT pos,gameboard& g){
    for(int dir=0;dir<6;dir++){
        if(!g.isFree(applayMove(pos,dir))){
            pieceT n=g.topPiece(applayMove(pos,dir));
            g.isValidMoveBitmask[
                15+dir+6*kind(p)+48*((n-1)%14)]=1;
                break;
        }
    }
    return movement(p,pos);
}   

actionT placeFirst(pieceT p){
    return (actionT)p;
}


actionT pass(){
    return 0;
}

