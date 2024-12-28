#ifndef POSITION_H
#define POSITION_H
#include <string>
#include <vector>
using namespace std;
#include "enums.h"
#include "direction.h"
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





#include "position.cpp"
#endif