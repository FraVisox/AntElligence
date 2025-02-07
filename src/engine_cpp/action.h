#ifndef ACTION_H
#define ACTION_H

#include "position.h"
#include "piece.h"
#include <unordered_set>

using namespace std;

enum TypeAction{
    MOVEMENT,
    PLACE,
    PLACEFIRST,
    PASS
};

class action{
    public:
        piece bug;
        position startingPos;
        TypeAction actType;

        piece otherBug;
        direction relativeDir;

        action(piece p,position d):bug(p),startingPos(d){}
        action(piece p):bug(p){}
        action(){}
};

extern action INVALID_ACTION;

action movement(piece p, piece other, direction dir);

action placePiece(piece p, piece other, direction d);

action placeFirst(piece p);

action pass();

action parseAction(string s, const unordered_set<piece>& inHandPiece);

string ActionToString(action a);

bool operator==(const action &a1,const action &a2);
#endif