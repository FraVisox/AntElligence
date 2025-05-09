#ifndef ACTION_H
#define ACTION_H

#include "position.h"
#include "piece.h"
#include "direction.h"
#include "gameboard.h"

//Types of actions

actionT movement(const pieceT p,const position &destPos,gameboard& G);
actionT placePiece(pieceT p,position pos, gameboard &G);
actionT placeFirst(pieceT p);
actionT pass();

//Parsing

//action parseAction(string s, const set<pieceT>& inHandPiece);
//string ActionToString(const action &a);

#endif