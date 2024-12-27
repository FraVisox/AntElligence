#include <bits/stdc++.h>
#include "piece.h"

using namespace std;

enum TypeAction{
    MOVEMENT,
    PLACE,
    PLACEFIRST,
    PASS
};

class action{
    public:
    TypeAction actType;
};

class movement:public action{
    public:
    piece bug;
    position pos;
    movement(piece p,position d):bug{p},pos{d} {
        this->actType=MOVEMENT;
    };
};

class placePiece:public action{
    public:
    piece bug;
    position pos;
    placePiece(piece p, position d):bug{p},pos{d}{
        this->actType=PLACE;}
};

class placeFirst:public action{
    public:
        piece bug;
        placeFirst(piece p):bug{p}{
        this->actType=PLACEFIRST;}
};

class pass:public action{};