#include "gameboard.h"
#include <iostream>
#include <cstring>


gameboard::gameboard(){
    memset(high, 0, sizeof(high));
    for(int i=0;i<32;i++)
        bugPosition[i]=NULL_POSITION;
    
    isPlaced.reset();
}


gameboard::gameboard(GameType gt){
    memset(high, 0, sizeof(high));
    for(int i=0;i<32;i++)
        bugPosition[i]=NULL_POSITION;
    isPlaced.reset();
}

/**
 * \brief Gets the positionT of a bug on the board.
 *
 * If the bug is not on the board, NULL_POSITION is returned.
 *
 * \param p The bug to get the positionT of.
 * \return The positionT of the bug on the board, NULL_POSITION if the bug is not on the board.
 */
positionT gameboard::getPosition(const pieceT &p){
    //if(bugPosition[p]!=10000  && bugPosition[p]>1023 )
    //    throw "Out of pos";
    return  bugPosition[p];

}

/**
 * \brief Updates the positionT of a bug on the gameboard.
 *
 * This function updates the positionT of the specified bug in the bugPosition map.
 * If the bug is already present in the map, its positionT is updated to the new
 * positionT provided. If the bug is not present, it is added to the map with the
 * given position.
 *
 * \param bug The bug whose positionT is to be updated.
 * \param pos The new positionT of the bug.
 */

void gameboard::updatePos(const pieceT &bug, const positionT &pos){
    bugPosition[bug]=pos;
}



/**
 * \brief Pops the top piece from the stack at the given position.
 *
 * If the stack at the given positionT is not empty, the top piece is removed from
 * the stack and its positionT in the bugPosition map is set to NULL_POSITION. If
 * the stack is empty, the positionT is removed from the occupied set.
 *
 * \param pos The positionT at which to pop the top piece.
 */
void gameboard::popPosition(const positionT &pos){
    if(!isFree(pos)){
        updatePos(topPiece(pos),NULL_POSITION);
        high[pos&1023]--;
    }
    if(isFree(pos))
        occupied.set(pos,0);
}


/**
 * \brief Removes a bug from the gameboard.
 *
 * If the bug is the top piece at its current position, it is removed from the
 * stack and its positionT in the bugPosition map is set to NULL_POSITION. If
 * the bug is not the top piece, it is simply removed from the bugPosition map.
 *
 * \param b The bug to be removed from the gameboard.
 */
void gameboard::removePiece(const pieceT &b){
    //if(isTop(b)){
        isPlaced.set(b,0);
        popPosition(getPosition(b));
        bugPosition[b]=NULL_POSITION;
    //}else{
    //    throw "Removing a non-top piece";
    //}
}

/**
 * \brief Adds a bug to the gameboard at the specified position.
 *
 * Places a bug on the gameboard at the specified position. The bug is added to
 * the top of the stack at that positionT and its positionT in the bugPosition map
 * is updated to the given position. The positionT is added to the occupied set.
 *
 * \param pos The positionT at which to add the bug.
 * \param b The bug to be added to the gameboard.
 */
void gameboard::addPiece(const positionT &pos, const pieceT &b){
    //if(kind(b)!=BugType::BEETLE && kind(b)!=BugType::MOSQUITO && !isFree(pos))throw "Adding over not empty";
    isPlaced.set(b,1);
    gb[getHight(pos)][pos&1023]=b;
    high[pos]++;
    updatePos(b,pos);
    occupied.set(pos ,1);
}


/**
 * \brief Checks if the specified positionT on the gameboard is free.
 *
 * This function determines whether the stack at the given positionT is empty,
 * indicating that no pieces are currently placed there.
 *
 * \param pos The positionT to check on the gameboard.
 * \return True if the positionT is free (no pieces present), false otherwise.
 */
bool gameboard::isFree(const positionT &pos){
    return getHight(pos)==0;
}


void gameboard::copy(gameboard& g){
    for(int i=0;i<32;i++)
        this->bugPosition[i]=g.bugPosition[i];

    for(int i=0;i<1024;i++){
        for(int h=0;h<HIGHT_BOARD;h++)
            this->gb[h][i]=g.gb[h][i];
        this->high[i]=g.high[i];
    }
    this->occupied=g.occupied;
    this->isPlaced=g.isPlaced;
    //for(int i=0;i<TOTAL_POSSIBLE_MOVES;i++)
    //    this->isValidMoveBitmask[i]=g.isValidMoveBitmask[i];
}
/**
 * \brief Checks if the given bug is at the top of the stack.
 *
 * This function checks if the given bug is at the top of the stack at its
 * current position. This is used to check if a bug can be moved, as only the
 * top bug at any given positionT can be moved.
 *
 * \param bug The bug to check.
 * \return True if the bug is at the top of the stack, false otherwise.
 */
bool gameboard::isTop(const pieceT &bug){
    positionT pos=getPosition(bug);
    return (topPiece(pos)==bug);
}

/**
 * \brief Checks if a bug piece can be moved.
 *
 * This function checks if a bug piece can be moved. A bug can be moved if it is
 * at the top of its stack and either not at level 1 or it is at level 1 and can
 * be moved without breaking the hive rule.
 *
 * \param b The bug to check.
 * \param turn The turn of the player.
 * \return True if the bug can be moved, false otherwise.
 */
bool gameboard::canPieceMove(const pieceT &b,int turn){
    return (isTop(b) && ((!isAtLevel1(getPosition(b))) || canMoveWithoutBreakingHiveRule(b,turn)));
}


int gameboard::getHight(const positionT &pos){
    return high[pos &1023];
}
/**
 * \brief Checks if a bug piece can be moved horizontally.
 *
 * This function checks if a bug piece can be moved horizontally. A bug can be
 * moved horizontally if it is at the top of its stack, the destination is at
 * level 1 and the neighboring positions are at a lower level than the source
 * position, and the source positionT is at level 1 or not all neighboring
 * positions are at a lower level than the source position.
 *
 * \param from The source positionT of the bug.
 * \param to The destination positionT of the bug.
 * \return True if the bug can be moved horizontally, false otherwise.
 */





bool gameboard::canSlideToFreeDir(const positionT &from,const positionT &to, direction n){

    positionT p1=applayMove(from,(n+1)%6);
    positionT p2=applayMove(from,(n+5)%6);
    int minH=min(getHight(from),getHight(to));

    return ((!(getHight(p1)>minH && getHight(p2)>minH))&&((!isFree(p1))|| (!isFree(p2))));


}

/**
 * \brief Gets the top piece on the given position.
 *
 * This function takes a positionT on the board and returns the top piece on
 * that position. If the positionT is empty, it throws a string with the
 * message "Asked piece for empty pos".
 *
 * \param pos The positionT to get the top piece from.
 * \return The top piece at the given positionT if it is not empty.
 * \throw A string with the message "Asked piece for empty pos" if the position
 *        is empty.
 */
pieceT gameboard::topPiece(const positionT &pos){
    //if(!isFree(pos)){
        return gb[getHight(pos)-1][pos &1023];
    //}
    //throw "Asked piece for empty pos";
}

pieceT gameboard::basePiece(const positionT &pos){
    return gb[0][pos &1023];
}


bitset<308> gameboard::toHash(){
    bitset<308> bs;
    for(int i=0;i<27;i++){
        pieceT b=i+1;
        int rb=0;
        bool ip=isPlaced[b];
        if(!ip){
            rb=1024;
        }else{
            positionT pos=getPosition(b);
            int h0=(basePiece(pos)==b);
            if(h0){
                rb=pos;
            }else{
                int j=1;
                while(gb[j][pos &1023]!= b) j++;
                rb=gb[j-1][pos &1023]+1024;
            }
        }
        bs=(bs<<11);
        bs|=rb;
    }
    return bs;
}

/**
 * \brief Gets all valid positions and directions to place a new piece on the board.
 *
 * This function takes a player color and returns a vector of pairs containing the
 * piece that can be placed and the direction from the adjacent occupied position.
 * The positions are only valid if they are free and if the adjacent neighbors are
 * all of the same color or free.
 *
 * \param color The player color to place the piece for.
 * \return A vector of pairs containing the piece and direction for each valid
 *         positionT to place a new piece.
 */
void gameboard::computeValidPositionPlaceNew(PlayerColor color){  // there is a faster way using bit map, but is boring
    numValidPosition=0;
    BoardBitSet colEq,canPlace(~occupied);
    
    //bitset<1024> hasOneNearSame,hasOneNearOpp;
    
    
    //hasOneNearSame.reset();
    //hasOneNearOpp.reset();

    //positionT pos;

/*
    for(int p=1;p<=28;p++){
        if(isPlaced[p] && isTop(p)){
            pos=getPosition(p);
            if(col(p)==color){
                for(int i=0;i<6;i++){
                    hasOneNearSame.set(applayMove(pos,i) ,true);
                }
            }else{
                for(int i=0;i<6;i++){
                    hasOneNearOpp.set(applayMove(pos,i) ,true);
                }
            } 
        }
    }

*/


    BoardBitSet  hons,hono;
    for(int p=1;p<=28;p++){
        if(isPlaced[p] && isTop(p)){
            colEq.set(getPosition(p),(col(p)==color));
        }
    }
    
    BoardBitSet xbbs=(occupied &colEq);
    BoardBitSet ybbs=(occupied & ~colEq);
        
    for(int d=0;d<6;d++){
        hons.updateOr(xbbs.getRot(d));
        hono.updateOr(ybbs.getRot(d));
    }
    /*
    for(int d=0;d<6;d++){
        hons|=sameCN[d];
    }
    for(int d=0;d<6;d++){
        hono|=diffCN[d];
    }


    if(hono!=hasOneNearOpp){
        cout<<"NO HONO";
        throw "NOT Same HONO";
    }
    if(hons!=hasOneNearSame){
        cout<<"NO HONS";
        throw "NOT Same HONS";
    }
*/

    canPlace.updateAnd(hons);
    canPlace.updateAnd(~hono);
    for(unsigned int IPos=0;IPos<1024;IPos++){
        if(canPlace.get_bit(IPos)){
            validPositionPlaceBuffer[numValidPosition]=IPos;
            numValidPosition++;
        }
    }
    //cout<<numValidPosition<<endl;
}


/**
 * \brief Checks if a positionT is at level 1.
 *
 * This function takes a positionT and checks if the stack of pieces at that
 * positionT has only one element. If the positionT is empty, it returns false.
 *
 * \param pos The positionT to check.
 * \return True if the positionT is at level 1, false otherwise.
 */
bool gameboard::isAtLevel1(const positionT &pos){
    return getHight(pos)==1;
}

/**
 * \brief Checks if a bug piece can be moved without breaking the hive rule.
 *
 * This function takes a bug piece and a turn and checks if the bug can be moved
 * without breaking the hive rule. The hive rule is broken if the bug is removed
 * from its current positionT and there is no other bug piece of the same player
 * that can reach the removed bug piece by a series of jumps. This function uses
 * a cache to avoid calculating the same thing multiple times in a row.
 *
 * \param b The bug piece to check.
 * \param turn The turn of the player.
 * \return True if the bug can be moved without breaking the hive rule, false otherwise.
 */

bool gameboard::canMoveWithoutBreakingHiveRule(const pieceT &b,int turn){
    if(! isPlaced[b]) //No position
        return true;
    //If there is a bug under this one
    positionT p=getPosition(b);
    if(getHight(p)>2) 
        return true;

    //Update cache
    if(turn != lastUpdateTurn){
        lastUpdateTurn = turn;
        find_articulation();
    }

    return not_movable_position[getPosition(b) ] == 0;
}