#ifndef ACTION_H
#define ACTION_H

#include "position.h"
#include "piece.h"
#include "direction.h"
#include <set>


enum TypeAction{
    MOVEMENT,
    PLACE,
    PLACEFIRST,
    PASS
};

class action{
    public:
        TypeAction actType;

        //Bug that moves and its position
        pieceT bug;
        position startingPos;

        //Relative direction to the other bug
        pieceT otherBug;
        direction relativeDir;

        //Constructors
        action(pieceT p,position d):bug(p),startingPos(d){}
        action(pieceT p):bug(p){}
        action(){}
};

extern action INVALID_ACTION;

//Types of actions

action movement(pieceT p, pieceT other, direction dir);
action placePiece(pieceT p, pieceT other, direction d);
action placeFirst(pieceT p);
action pass();

//Parsing

//action parseAction(string s, const set<pieceT>& inHandPiece);
string ActionToString(const action &a);

bool operator==(const action &a1,const action &a2);
#endif