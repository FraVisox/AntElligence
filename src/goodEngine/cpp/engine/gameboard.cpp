#include "gameboard.h"
#include <iostream>

#include "../graph_board/graph_board.h"


gameboard::gameboard(){
    memset(high, 0, sizeof(high));
    for(int i=0;i<32;i++)
        bugPosition[i]=NULL_POSITION;
    
    occupied.reset();
    isPlaced.reset();
}

/**
 * \brief Gets the position of a bug on the board.
 *
 * If the bug is not on the board, NULL_POSITION is returned.
 *
 * \param p The bug to get the position of.
 * \return The position of the bug on the board, NULL_POSITION if the bug is not on the board.
 */
position gameboard::getPosition(const pieceT &p){
    if(bugPosition[p].first!=10000  && bugPosition[p].first>=32 && bugPosition[p].first<0)
        throw "Out of pos";
    if(bugPosition[p].second!=10000  && bugPosition[p].second>=32 && bugPosition[p].second<0)
        throw "Out of pos";
    return  bugPosition[p];

}

/**
 * \brief Updates the position of a bug on the gameboard.
 *
 * This function updates the position of the specified bug in the bugPosition map.
 * If the bug is already present in the map, its position is updated to the new
 * position provided. If the bug is not present, it is added to the map with the
 * given position.
 *
 * \param bug The bug whose position is to be updated.
 * \param pos The new position of the bug.
 */

void gameboard::updatePos(const pieceT &bug, const position &pos){
    bugPosition[bug]=pos;
}



/**
 * \brief Pops the top piece from the stack at the given position.
 *
 * If the stack at the given position is not empty, the top piece is removed from
 * the stack and its position in the bugPosition map is set to NULL_POSITION. If
 * the stack is empty, the position is removed from the occupied set.
 *
 * \param pos The position at which to pop the top piece.
 */
void gameboard::popPosition(const position &pos){
    if(!isFree(pos)){
        updatePos(topPiece(pos),NULL_POSITION);
        high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD]--;
    }
    if(isFree(pos))
        occupied.set(pos.toInt(),0);
}


/**
 * \brief Removes a bug from the gameboard.
 *
 * If the bug is the top piece at its current position, it is removed from the
 * stack and its position in the bugPosition map is set to NULL_POSITION. If
 * the bug is not the top piece, it is simply removed from the bugPosition map.
 *
 * \param b The bug to be removed from the gameboard.
 */
void gameboard::removePiece(const pieceT &b){
    if(isTop(b)){
        isPlaced.set(b,0);
        popPosition(getPosition(b));
        bugPosition[b]=NULL_POSITION;
    }else{
        throw "Removing a non-top piece";
    }
}

/**
 * \brief Adds a bug to the gameboard at the specified position.
 *
 * Places a bug on the gameboard at the specified position. The bug is added to
 * the top of the stack at that position and its position in the bugPosition map
 * is updated to the given position. The position is added to the occupied set.
 *
 * \param pos The position at which to add the bug.
 * \param b The bug to be added to the gameboard.
 */
void gameboard::addPiece(const position &pos, const pieceT &b){
    if(kind(b)!=BugType::BEETLE && !isFree(pos))throw "Adding over not empty";
    isPlaced.set(b,1);
    gb[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD][getHight(pos)]=b;
    high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD]++;
    updatePos(b,pos);
    occupied.set(pos.toInt(),1);
}


/**
 * \brief Checks if the specified position on the gameboard is free.
 *
 * This function determines whether the stack at the given position is empty,
 * indicating that no pieces are currently placed there.
 *
 * \param pos The position to check on the gameboard.
 * \return True if the position is free (no pieces present), false otherwise.
 */
bool gameboard::isFree(const position &pos){
    return getHight(pos)==0;
}


void gameboard::copy(gameboard& g){
    for(int i=0;i<32;i++)
        this->bugPosition[i]=g.bugPosition[i];

    for(int i=0;i<SIZE_BOARD;i++)
        for(int j=0;j<SIZE_BOARD;j++){
            for(int h=0;h<HIGHT_BOARD;h++)
                this->gb[i][j][h]=g.gb[i][j][h];
            this->high[i][j]=g.high[i][j];
    }
    this->occupied=g.occupied;
    this->isPlaced=g.isPlaced;
}
/**
 * \brief Checks if the given bug is at the top of the stack.
 *
 * This function checks if the given bug is at the top of the stack at its
 * current position. This is used to check if a bug can be moved, as only the
 * top bug at any given position can be moved.
 *
 * \param bug The bug to check.
 * \return True if the bug is at the top of the stack, false otherwise.
 */
bool gameboard::isTop(const pieceT &bug){
    position pos=getPosition(bug);
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


int gameboard::getHight(const position &pos){
    return high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD];
}
/**
 * \brief Checks if a bug piece can be moved horizontally.
 *
 * This function checks if a bug piece can be moved horizontally. A bug can be
 * moved horizontally if it is at the top of its stack, the destination is at
 * level 1 and the neighboring positions are at a lower level than the source
 * position, and the source position is at level 1 or not all neighboring
 * positions are at a lower level than the source position.
 *
 * \param from The source position of the bug.
 * \param to The destination position of the bug.
 * \return True if the bug can be moved horizontally, false otherwise.
 */



bool gameboard::canSlideToFree( position &from, position to){
    return (!isGate(from,to)) && isJoined(from,to);
}


bool gameboard::canSlideToFreeDir(const position &from,const position &to, direction n){

    position p1=from.applayMove((n+1)%6);
    position p2=from.applayMove((n+5)%6);
    int minH=min(getHight(from),getHight(to));

    return ((!(getHight(p1)>minH && getHight(p2)>minH))&&((!isFree(p1))|| (!isFree(p2))));


}

bool gameboard::isGateDir(const position &from,const position &to,const direction n){
    position p1=from.applayMove((n+1)%6);
    position p2=from.applayMove((n+5)%6);
    int minH=min(getHight(from),getHight(to));
    return (getHight(p1)>minH && getHight(p2)>minH);
}

bool gameboard::isJoinedDir(const position &from,const position &to,const direction n){
    position p1=from.applayMove((n+1)%6);
    position p2=from.applayMove((n+5)%6);
    return ( (!isFree(p1)) || (!isFree(p2)));
}


bool gameboard::isGate(position &from,position &to){
    direction n = getMovementDirection(from,to);
    position p1=from.applayMove((n+1)%6);
    position p2=from.applayMove((n+5)%6);
    int minH=min(getHight(from),getHight(to));
    return (getHight(p1)>minH && getHight(p2)>minH);
}

bool gameboard::isJoined(position &from,position &to){
    direction n = getMovementDirection(from,to);
    position p1=from.applayMove((n+1)%6);
    position p2=from.applayMove((n+5)%6);
    return ( (!isFree(p1)) || (!isFree(p2)));
}

/**
 * \brief Finds the relative position if the bug at position 'from' can move to position 'to'.
 *
 * This function finds the relative position if the bug at position 'from' can move to position 'to'.
 * If the bug can be moved, it will return a pair containing the piece that is relative to the bug and the direction to move.
 * If the bug cannot be moved, it will return a pair containing the invalid piece and the invalid direction.
 *
 * \param to The destination position of the bug.
 * \param from The source position of the bug.
 * \param canOver Whether the bug can be moved over other pieces.
 * \return A pair containing the relative piece and direction if the bug can be moved, otherwise a pair containing the invalid piece and direction.
 */



// TOD FIX IT:
/*
bool gameboard::checIfCanMove(position &to, position &from, bool canOver){
    if(!isNear(to,from))return false;

    if(isAtLevel1(from) && canHorizontalSlide(from,to)){
            return true;
    }
    if (canOver && isNear(to, from) && !isFree(to)) {
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        //If I am climbing to reach that spot
        if(getHight(p1) <= getHight(to)|| getHight(p2) <= getHight(to)) {
            return true;
        }
    }
    if (isNear(to,from)) {
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        if (!isAtLevel1(from) && getHight(to) < getHight(from) && (getHight(p1) < getHight(from) || getHight(p2) < getHight(from))) {
            pieceT current = topPiece(from);
            high[(from.first+SIZE_BOARD)%SIZE_BOARD][(from.second+SIZE_BOARD)%SIZE_BOARD]--;
            pieceT other = topPiece(from);
            direction dir = getMovementDirection(from, to);
            gb[(from.first+SIZE_BOARD)%SIZE_BOARD][(from.second+SIZE_BOARD)%SIZE_BOARD][getHight(from)]=current;
            high[(from.first+SIZE_BOARD)%SIZE_BOARD][(from.second+SIZE_BOARD)%SIZE_BOARD]++;

            return true;
        }
    }
    return false;

    
}
*/
/**
 * \brief Determines the sliding move at level 1 from one position to another.
 *
 * This function checks if a move from the 'from' position to the 'to' position
 * is possible by sliding at level 1 on a hexagonal grid. It assesses the 
 * neighboring positions to determine if the move can occur by sliding around
 * an adjacent piece. If the target position is free and adjacent to the source 
 * position, it evaluates the neighbors to identify a valid sliding path.
 *
 * \param to The destination position.
 * \param from The starting position.
 * \return A pair containing the relative piece and direction for the slide if 
 *         a valid sliding move is possible, otherwise a pair containing the 
 *         invalid piece and direction.
 */
/*
pair<pieceT, direction> gameboard::getSlidingMoveAtLevel1(position &to, position &from) {
    if(isFree(to) && isNear(from, to)){
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        //If both are at an higher level than this one, we can't move
        if(isFree(p1) && !isFree(p2)) //I slide around p2
            return {topPiece(p2), getMovementDirection(p2, to)};
        else if(!isFree(p1) && isFree(p2)) //I slide around p1
            return {topPiece(p1), getMovementDirection(p1, to)};
    }
    return {INVALID_PIECE, INVALID};
}

*/
/**
 * \brief Finds the occupied adjacent positions of a given position.
 *
 * This function takes a position on the board and returns a vector of
 * positions that are adjacent to the given position and are occupied by
 * at least one bug.
 *
 * \param pos The position to check.
 * \return A vector of positions that are adjacent and occupied.
 */
vector<position> gameboard::occupiedEdge(position &pos){  // TO CHECK
    vector<position> ris;
    for(auto n: pos.neighbor()){
        if(!isFree(n)){
            ris.push_back(n);
        }
    }
    return ris;
}

/**
 * \brief Gets the top piece on the given position.
 *
 * This function takes a position on the board and returns the top piece on
 * that position. If the position is empty, it throws a string with the
 * message "Asked piece for empty pos".
 *
 * \param pos The position to get the top piece from.
 * \return The top piece at the given position if it is not empty.
 * \throw A string with the message "Asked piece for empty pos" if the position
 *        is empty.
 */
pieceT gameboard::topPiece(const position &pos){
    if(!isFree(pos)){
        return gb[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD][getHight(pos)-1];
    }
    throw "Asked piece for empty pos";
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
 *         position to place a new piece.
 */
vector<position> gameboard::validPositionPlaceNew(PlayerColor color){  // there is a faster way using bit map, but is boring
    bitset<SIZE_BOARD*SIZE_BOARD> hasOneNearSame, hasOneNearOpp;
    hasOneNearSame.reset();
    hasOneNearOpp.reset();
    
    vector<position> ris;
    for(int IPos=0;IPos<SIZE_BOARD*SIZE_BOARD;IPos++){
        if(occupied[IPos]){
            position pos(IPos);
            pieceT x=topPiece(pos);
            if(col(x)==color){
                for(int i=0;i<6;i++){
                    hasOneNearSame.set(pos.applayMove(i).toInt(),true);
                }
            }else{
                for(int i=0;i<6;i++){
                    hasOneNearOpp.set(pos.applayMove(i).toInt(),true);
                }
            }
        }
    }

    for(int IPos=0;IPos<SIZE_BOARD*SIZE_BOARD;IPos++){
        if((!occupied[IPos])  && (hasOneNearSame[IPos]) &&(!hasOneNearOpp[IPos])){
            ris.push_back(position{IPos});
        }
    }
    
    return ris;
}


/**
 * \brief Checks if a position is at level 1.
 *
 * This function takes a position and checks if the stack of pieces at that
 * position has only one element. If the position is empty, it returns false.
 *
 * \param pos The position to check.
 * \return True if the position is at level 1, false otherwise.
 */
bool gameboard::isAtLevel1(const position &pos){
    return getHight(pos)==1;
}

/**
 * \brief Checks if a bug piece can be moved without breaking the hive rule.
 *
 * This function takes a bug piece and a turn and checks if the bug can be moved
 * without breaking the hive rule. The hive rule is broken if the bug is removed
 * from its current position and there is no other bug piece of the same player
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
    position p=getPosition(b);
    if(getHight(p)>2) 
        return true;

    //Update cache
    if(turn != lastUpdateTurn){
        lastUpdateTurn = turn;
        find_articulation();
    }

    return not_movable_position[getPosition(b).toInt()] == 0;
}