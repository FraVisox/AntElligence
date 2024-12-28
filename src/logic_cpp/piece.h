#ifndef PIECE_H
#define PIECE_H
#include <string>
#include "enums.h"

class piece{
    public:
    PlayerColor col;
    int numIncr;
    BugType kind;
    piece(enum BugType k,PlayerColor c,int=0);
    piece(string s);
    piece(int val);
    piece(){};
    string toString() const;
    int toVal() const;
};

template<>
struct std::hash<piece>
{
    std::size_t operator()(const piece& p) const noexcept
    {
        return  hash<int>{}(p.toVal());
    }
};


bool operator!=(const piece& p1,const piece &p);
bool operator==(const piece& p1,const piece &p);

#include "piece.cpp"
#endif