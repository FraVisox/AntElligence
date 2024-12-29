#include "position.h"
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
    piece bug;
    position pos;
    TypeAction actType;
    action(piece p,position d):bug(p),pos(d){}
    action(piece p):bug(p){}
    action(){}
};

action movement(piece p,position d){
    action a;
    a.bug=p;
    a.pos=d;
    a.actType=MOVEMENT;
    return a;
}

action placePiece(piece p, position d){
    action a(p,d);
    a.actType=PLACE;
    return a;
}

action placeFirst(piece p){
    action a(p);
    a.actType=PLACEFIRST;
    return a;
}
action pass(){
    action a;
    return a;
}
