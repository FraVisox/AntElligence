#include <bits/stdc++.h>

#ifndef INCLUDE_ENUMS
#include "enums.cpp"
#endif
#define INCLUDE_ENUMSusing namespace std;


class piece{
    public:
    PlayerColor col;
    int numIncr;
    BugType kind;
    string name;

    piece(enum BugType k,PlayerColor c,int n=0){
        kind=k;
        col=c;
        numIncr=n;

        name=ColorString(c)+BugString(k)+(n==0?(""):(""+n));
    }
    piece(string s){
        this->name=s;
        col=StringColor(s.substr(0,1));
        kind=StringBug(s.substr(1,1));
        if(s.size()==3){
            numIncr=stoi(s.substr(2,1));
        }else{
            numIncr=0;
        }
    }

    
};

bool operator==(const piece& p1,const piece &p){
    return p1.name==p.name;
}

bool operator!=(const piece& p1,const piece &p){
    return (p1.name!=p.name);
}

template<>
struct std::hash<piece>
{
    std::size_t operator()(const piece& p) const noexcept
    {
        return  hash<string>{}(p.name);
    }
};