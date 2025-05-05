#ifndef POSITION_H
#define POSITION_H
#include <string>
#include <vector>
#include "enums.h"
#include "direction.h"
#define SIZE_BOARD 32

class position{
    public:
    unsigned int first;
    unsigned int second;

    position(unsigned int x,unsigned int y);
    position(const position &p);
    position();
    position(int k);

    //Relative positions

    vector<position> neighbor();
    position applayMove(direction d) const;
    unsigned int toInt() const;
};

extern position NULL_POSITION;

direction getMovementDirection(const position &f, const position &t);
bool isNear(const position &p1, const position &p2);
vector<position> nearBoth(position &p1, position &p2);

bool operator==(const position &p1,const position& p2);
bool operator!=(const position &p1,const position& p2);
bool operator<(const position &p1,const position& p2);




#endif