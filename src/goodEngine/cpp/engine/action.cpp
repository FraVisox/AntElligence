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
actionT movement(pieceT p,position destPos,gameboard& G){
    actionT r=(actionT)p;
    if(destPos.first==10000)
        throw "Cannot put here";

    int resHight=G.getHight(destPos);

    for(int j=0;j<6;j++){
        position posNearJ=destPos.applayMove((j));
        if(G.getHight(posNearJ)>resHight){
            pieceT nextBug=G.gb[(posNearJ.first+SIZE_BOARD)%SIZE_BOARD][(posNearJ.second+SIZE_BOARD)%SIZE_BOARD][resHight];
            if(nextBug!=p)
                r|=((actionT)(nextBug))<<((actionT)(8*(j+1)));
        }
    }

    if(resHight!=0){
        r|=((actionT)(((actionT)(G.topPiece(destPos)))))<<(56);
    }
    return r;
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
actionT placePiece(pieceT p,position pos,gameboard& g){
    for(int dir=0;dir<6;dir++){
        if(!g.isFree(pos.applayMove(dir))){
            pieceT n=g.topPiece(pos.applayMove(dir));
            g.isValidMoveBitmask.set(
                15+dir+6*kind(p)+48*((n-1)%14),
                1
            );
        }
    }
    return movement(p,pos,g);
}   

/**
 *   Constructs a place first action.
 * 
 *   :param p: Bug piece to be placed.
 *   :type p: piece
 *   :return: Action representing the placement of the first bug.
 *   :rtype: action
 */
actionT placeFirst(pieceT p){
    return (actionT)p;
}



/**
 *   Constructs a pass action.
 * 
 *   :return: Action representing the pass of the player's turn.
 *   :rtype: action
 */
actionT pass(){
    return 0;
}

