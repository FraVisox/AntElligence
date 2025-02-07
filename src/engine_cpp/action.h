#ifndef ACTION_H
#define ACTION_H

#include "position.h"
#include "piece.h"
#include "direction.h"
#include <unordered_set>


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
        piece bug;
        position startingPos;

        //Relative direction to the other bug
        piece otherBug;
        direction relativeDir;

        //Constructors
        action(piece p,position d):bug(p),startingPos(d){}
        action(piece p):bug(p){}
        action(){}
};

extern action INVALID_ACTION;

//Types of actions

action movement(piece p, piece other, direction dir);
action placePiece(piece p, piece other, direction d);
action placeFirst(piece p);
action pass();

//Parsing

action parseAction(string s, const unordered_set<piece>& inHandPiece);
string ActionToString(const action &a);

#endif