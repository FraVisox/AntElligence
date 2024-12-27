#ifndef POSITION_H
#define POSITION_H
#include <bits/stdc++.h>
using namespace std;
#include "enums.h"

class position{
    public:
    int first;
    int second;

    position(int x,int y);
    position(const position &p);
    
    position();
    void normalize();

    vector<position> neighbor();

    position applayMove(Direction d);
};

position NULL_POSITION(10000,10000);



Direction getMovementDirection(position f, position t);

pair<int,int> associedDifference(Direction d);
int directionToNumber(Direction d);
Direction numberToDirection(int n);
bool operator==(const position &p1,const position& p2);

bool operator!=(const position &p1,const position& p2);

bool isNear(const position &p1, const position &p2);


vector<position> otherWay(position &p1,position &p2);


template<>
struct std::hash<position>
{
    std::size_t operator()(const position& p) const noexcept
    {
        return  hash<int>{}(p.first+1000*p.second);
    }
};


pair<int,int> movementCircleClockwise[]={
    make_pair(+2,0),
    make_pair(+1,+1),
    make_pair(-1,+1),
    make_pair(-2,-1),
    make_pair(-1,-1),
    make_pair(+1,-1)
};


Direction allDirections[]={RIGHT,DOWN_RIGHT,DOWN_LEFT,LEFT,UP_LEFT,UP_RIGHT};

#endif