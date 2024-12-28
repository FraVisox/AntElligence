#ifndef DIRECTION_H
#define DIRECTION_H
#include <utility>
using namespace std;

enum Direction{
    RIGHT=0,
    DOWN_RIGHT=1,
    DOWN_LEFT=2,
    LEFT=3,
    UP_LEFT=4,
    UP_RIGHT=5
};


Direction allDirections[]={RIGHT,DOWN_RIGHT,DOWN_LEFT,LEFT,UP_LEFT,UP_RIGHT};

pair<int,int> movementCircleClockwise[]={
    make_pair(+2,0),
    make_pair(+1,+1),
    make_pair(-1,+1),
    make_pair(-2,-1),
    make_pair(-1,-1),
    make_pair(+1,-1)
};


// i want to move in the direction base on the bug name
string stringNameDir(string name,Direction dir){
    switch (dir)
    {
        case RIGHT: return name+"-";
        case DOWN_RIGHT: return name+"\\";
        case DOWN_LEFT: return "/"+name;
        case LEFT: return "-"+name;
        case UP_LEFT: return "\\"+name;
        case UP_RIGHT: return name+"/";
    }
    return "";
}



Direction numberToDirection(int n){
    return allDirections[n];
}
Direction oppositeDir(Direction d){
    return numberToDirection((d+3)%6);
}


pair<int,int> associedDifference(Direction d){
    return movementCircleClockwise[d];
}

#endif