#include "gameboard.h"
#include <iostream>
#include <cstring>


gameboard::gameboard(){
    memset(high, 0, sizeof(high));

    hasUpdatedArticulation=false;
    isPlaced.reset();
}



gameboard::gameboard(GameType gt){
    memset(high, 0, sizeof(high));

    hasUpdatedArticulation=false;
    isPlaced.reset();
}

positionT gameboard::getPosition(const pieceT &p) const {
    return  bugPosition[p];
}




 

void gameboard::removePiece(const pieceT &b){
    positionT pos=getPosition(b);
    isPlaced.set(b,0);
    high[pos &1023]--;
    if(isFree(pos))
        occupied.set(pos,0);

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
    bugPosition[b]=pos;
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
    for(int i=0;i<16;i++){
        this->occupied.bv[i]=g.occupied.bv[i];
    }
    
    this->isPlaced=g.isPlaced;
    
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
bool gameboard::isTop(const pieceT &bug) const {
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


int gameboard::getHight(const positionT &pos) const {
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
pieceT gameboard::topPiece(const positionT &pos) const {
    //if(!isFree(pos)){
        return gb[getHight(pos)-1][pos &1023];
    //}
    //std::cerr<<"Error in gameboard, in top Piece"<<std::endl;
    //throw "Asked piece for empty pos";
}

pieceT gameboard::basePiece(const positionT &pos){
    return gb[0][pos &1023];
}


bitset<285> gameboard::toHash(){
    int numPlaced=0;
    bitset<285> bs(0);
    for(int i=0;i<=27;i++){
        pieceT b=i+1;
        
        if(isPlaced[b]){
            numPlaced++;
            positionT pos=getPosition(b);
            
            for(int j=0;j<10;j++){
                bs[i*10+j]=(pos>>j)&1;
            }
        }
    }
    for(int j=0;j<5;j++){
        bs[280+j]=(numPlaced>>j)&1;
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


    canPlace.updateAnd(hons);
    canPlace.updateAnd(~hono);

    for(uint64_t i=0;i<16;i++){
        if(canPlace.bv[i]>0){
            for(uint64_t j=0;j<64;j++){
                if(canPlace.bv[i]&(1ull<<(63-j))){

                    validPositionPlaceBuffer[numValidPosition]=(i<<6)|(j);
                    numValidPosition++;
                }
            }
        }
    }
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
    if(!hasUpdatedArticulation){
        hasUpdatedArticulation =true;
        find_articulation();
    }

    return not_movable_position.get_bit(p) == 0;
}