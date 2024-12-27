#include <bits/stdc++.h>
#ifndef PIECE_H
#define PIECE_H
#include "position.h"
#include "enums.h"

class piece{
    public:
    position placed;
    PlayerColor col;
    int numIncr;
    BugType kind;
    string name;
    piece(enum BugType k,PlayerColor c,int n=0);
    piece(string s);
};

template<>
struct std::hash<piece>
{
    std::size_t operator()(const piece& p) const noexcept
    {
        return  hash<string>{}(p.name);
    }
};
#endif