#ifndef ACTION_H
#define ACTION_H

#include "position.h"
#include "piece.h"
#include "direction.h"
#include "gameboard.h"

//Types of actions

actionT movement(const pieceT p,const positionT &destPos);
actionT placePiece(pieceT p,positionT pos, gameboard &G);
actionT placeFirst(pieceT p);
actionT pass();

//Parsing


#endif