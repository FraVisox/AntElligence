#include "gameboard.h"
#include <iostream>

/**
 * Resets the gameboard to its initial state.
 *
 * All positions on the board are cleared, and all the data structures are cleared.
 * This should be called at the start of each game.
 */
void gameboard::reset() {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            gb[i][j] = stack<piece>();
        }
    }
    bugPosition.clear();
    occupied.clear();
}

/**
 * \brief Get the stack of pieces at a given position.
 *
 * \param pos The position to get the stack of pieces from.
 * \return A pointer to the stack of pieces at pos.
 */
stack<piece>* gameboard::at(position pos){
    return &gb[(100+(pos.first%100))%100][(100+(pos.second%100))%100];
}

/**
 * \brief Get the position of a bug on the board.
 *
 * \param p The bug to get the position of.
 * \return The position of bug p.
 */
position gameboard::getPosition(piece p){
    return bugPosition.at(p);
}

/**
 * \brief Update the position of a bug on the board.
 *
 * \param bug The bug to update the position of.
 * \param pos The new position of bug.
 */
void gameboard::updatePos(piece bug, const position &pos){
    bugPosition.insert_or_assign(bug,pos);
}


/**
 * \brief Remove the top piece from a given position and update the
 *        occupied set.
 *
 * If the position is not free, remove the top piece from the position
 * and update the position of the removed piece to NULL_POSITION.
 * Then, if the position is free after popping the top piece, remove
 * the position from the occupied set.
 *
 * \param pos The position to pop the top piece from.
 */
void gameboard::popPosition(position pos){
    if(!isFree(pos)){
        updatePos(at(pos)->top(), NULL_POSITION);
        at(pos)->pop();
    }
    if(isFree(pos))
        occupied.extract(pos);
}


/**
 * \brief Remove a bug from the board.
 *
 * If the bug is the top piece at its position, remove it from the board.
 * This involves popping the piece from its position and removing its
 * position from the occupied set. Additionally, remove the bug from
 * the bugPosition map.
 *
 * \param b The bug to remove from the board.
 */
void gameboard::removePiece(piece b){
    if(isTop(b)){
        popPosition(getPosition(b));
        bugPosition.extract(b);
    }
}

/**
 * \brief Add a piece to the board.
 *
 * Pushes the given bug onto the given position and updates the bug's
 * position in the bugPosition map. Additionally, inserts the position
 * into the occupied set.
 *
 * \param pos The position to add the piece to.
 * \param a The action containing the info to add the piece.
 */
void gameboard::addPiece(action a){
    position pos;
    switch (a.actType) {
        case PLACEFIRST:
            pos = position{0,0};
            break;
        case PLACE:
        case MOVEMENT:
            pos = calcPosition(a.otherBug, a.relativeDir);
            break;
        case PASS:
            return;
    }
    cout << "Adding " << a.bug.toString() << " at " << pos.first << " " << pos.second << endl;
    piece b = a.bug;
    at(pos)->push(b);
    updatePos(b,pos);
    occupied.insert(pos);
}


/**
 * \brief Add a piece to the board at a given position.
 *
 * Pushes the given bug onto the given position and updates the bug's
 * position in the bugPosition map. Additionally, inserts the position
 * into the occupied set.
 *
 * \param pos The position to add the piece to.
 * \param b The bug to add to the board.
 */
void gameboard::addPiece(position pos, piece b){
    at(pos)->push(b);
    updatePos(b,pos);
    occupied.insert(pos);
}


/**
 * \brief Check if a position is free of pieces.
 *
 * \param pos The position to check.
 * \return true if the position is free, false otherwise.
 */
bool gameboard::isFree(position &pos){
    return at(pos)->empty();
}

/**
 * \brief Check if the given bug is the top piece at its position.
 *
 * \param bug The bug to check.
 * \return true if the bug is the top piece at its position, false otherwise.
 */
bool gameboard::isTop(piece bug){
    position pos=getPosition(bug);
    return (at(pos)->top()==bug);
}

/**
 * \brief Check if the given bug can move on the given turn.
 *
 * Checks if the given bug is the top piece at its position and if the
 * hive rule is satisfied for the given bug on the given turn.
 *
 * \param b The bug to check.
 * \param turn The turn to check.
 * \return true if the bug can move on the given turn, false otherwise.
 */
bool gameboard::canPieceMove(piece b,int turn){
    return (isTop(b) && canMoveWithoutBreakingHiveRule(b,turn));
}

/**
 * \brief Check if a bug can slide to the given position.
 *
 * A bug can slide to the given position if the given position is
 * free, the bug is at a position adjacent to the given position, and the
 * two positions on the path between the bug and the given position are
 * both free.
 *
 * \param from The position of the bug.
 * \param to The position the bug would like to slide to.
 * \return true if the bug can slide to the given position for free, false otherwise.
 */
bool gameboard::canHorizontalSlide(position from, position to){
    if(at(to)->size() < at(from)->size() && isNear(to, from)){
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        //If both are at an higher level than this one, we can't move
        if(at(p1)->size() >= at(from)->size() && at(p2)->size() >= at(from)->size())
            return false;
        return true;
    }
    return false;
}


/**
 * \brief Determine the relative position and direction for a possible move.
 * 
 * This function evaluates whether a piece can move from a starting position
 * to a target position on the gameboard. Depending on the conditions, it
 * returns the top piece and direction around which the movement can occur.
 * 
 * If the piece is at level 1 and can slide horizontally, it checks the
 * neighboring positions to determine the best path for sliding around.
 * Alternatively, if the piece can move over others and is near the target
 * position, it checks if the piece can move over to the target position.
 * 
 * \param to The target position the piece wants to move to.
 * \param from The starting position of the piece.
 * \param canOver Whether the piece is allowed to move over other pieces.
 * \param howMuchOverFrom The additional height from which the piece can move over.
 * \return A pair containing the top piece at the relevant position and
 *         the direction for the move. Returns INVALID_PIECE and INVALID
 *         direction if the move is not possible
 */
pair<piece, direction> gameboard::getRelativePositionIfCanMove(position to, position from, bool canOver){

    //TODO: check the logic of this function

    //If I'm at level 1
    if(isAtLevel1(from) && canHorizontalSlide(from,to)){
        vector<position> v = nearBoth(to,from);
        position p1 = v[0], p2 = v[1];
        if(isFree(p1) && !isFree(p2)) //I slide around p2
            return {at(p2)->top(), getMovementDirection(p2, to)};
        else if(!isFree(p1) && isFree(p2)) //I slide around p1
            return {at(p1)->top(), getMovementDirection(p1, to)};
    } 
    else if (canOver && isNear(to, from)) {
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        //If I am climbing to reach that spot
        if(at(p1)->size() < at(to)->size() || at(p2)->size() < at(to)->size()) {
            return {at(to)->top(), OVER};
        }
        
    }
    //If I am coming down from an upper position
    else if (isNear(to,from)) {
        vector<position> v= nearBoth(from,to);
        position p1=v[0], p2=v[1];
        if (!isAtLevel1(from) && at(to)->size() < at(from)->size() && (at(p1)->size() < at(from)->size() || at(p2)->size() < at(from)->size())) {
            piece current = at(from)->top();
            at(from)->pop();
            piece other = at(from)->top();
            direction dir = getMovementDirection(from, to);
            at(from)->push(current);
            return {other, dir};
        }
    }
    return {INVALID_PIECE, INVALID};
}

/**
 * \brief Get the occupied neighboring positions.
 *
 * This function returns a vector of positions that are neighbors
 * to the given position and are currently occupied by pieces.
 *
 * \param pos The position to check for occupied neighbors.
 * \return A vector containing the positions of all occupied neighbors.
 */

vector<position> gameboard::occupiedEdge(position pos){
    vector<position> ris;
    for(auto n:pos.neighbor()){
        if(!isFree(n)){
            ris.push_back(n);
        }
    }
    return ris;
}

/**
 * \brief Get the top piece at a given position.
 *
 * \param pos The position to get the top piece from.
 * \return The top piece at the given position.
 *
 * \throws std::string if the given position is empty.
 */
piece gameboard::topPiece(position pos){
    if(!isFree(pos)){
        return at(pos)->top();
    }
    throw "Asked piece for empty pos";
}


/**
 * \brief Get all valid positions to place a new piece.
 *
 * A position is valid if it is adjacent to a piece of the given color, and
 * if all the neighbors of the position are either free or of the same color.
 *
 * \param color The color of the player for which to get the valid positions.
 * \return An unordered set of all valid positions to place a new piece.
 */
vector<pair<piece,direction>> gameboard::validPositionPlaceNew(PlayerColor color){
    unordered_set<position> seen;
    vector<pair<piece,direction>> valid;
    for(position op : occupied){
        piece p = at(op)->top();
        if(p.col == color){
            for(position possiblePosition:op.neighbor()){
                if(seen.count(possiblePosition)) 
                    continue;

                seen.insert(possiblePosition);

                if(isFree(possiblePosition)){
                    bool hasOtherColor=false;
                    for(auto n:possiblePosition.neighbor()){
                        if(!isFree(n) && at(n)->top().col!=color){
                            hasOtherColor=true;
                            break;
                        }
                    }
                    if(!hasOtherColor){
                        valid.push_back(make_pair(p,getMovementDirection(op, possiblePosition)));
                    }
                }
            }
        }
    }
    return valid;
}


/**
 * \brief Check if a position is at level 1.
 *
 * A position is at level 1 if there is only one piece at that position.
 *
 * \param pos The position to check.
 * \return true if the position is at level 1, false otherwise.
 */
bool gameboard::isAtLevel1(position pos){
    return at(pos)->size()==1;
}

/**
 * \brief Check if the bug can be moved.
 *
 * A bug can be moved if it is possible to move it without breaking
 * the hive rule.
 *
 * \param b The bug to check.
 * \param turn The current turn.
 * \return true if the bug is part of the hive, false otherwise.
 */
bool gameboard::canMoveWithoutBreakingHiveRule(piece b,int turn){
    if(getPosition(b)==NULL_POSITION) //No position
        return true;
    //If there is a bug under this one
    if(at(getPosition(b))->size()>2) 
        return true;

    //Update cache
    if(turn != lastUpdateTurn){
        lastUpdateTurn = turn;
        find_articulation();
    }

    return not_movable_position.count(getPosition(b)) == 0;
}