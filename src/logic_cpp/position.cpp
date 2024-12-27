#include <bits/stdc++.h>

#ifndef INCLUDE_ENUMS
#include "enums.cpp"
#endif
#define INCLUDE_ENUMS
using namespace std;

/*
implement as orizontal rectangualr with base 0,0:



-------------------------------------
|           |           |
| 0,0   1,0 | 2,0   3,0 | 4,0   5,0
|           |           |     
-------------------------------------
      |           |           |        
  0,1 | 1,1   2,1 | 3,1   4,1 | 5,1
      |           |           |    
-------------------------------------   
|           |           |
| 0,2   1,2 | 2,2   3,2 | 4,2   5,2
|           |           | 
-------------------------------------
      |           |           |        
  0,3 | 1,3   2,3 | 3,3   4,3 | 5,3
      |           |           |    
-------------------------------------   
|           |           |
| 0,4   1,4 | 2,4   3,4 | 4,4   5,4
|           |           | 
-------------------------------------
      |           |           |        
  0,5 | 1,5   2,5 | 3,5   4,5 | 5,5
      |           |           |    
-------------------------------------   


*/


class position{
    public:
    int first;
    int second;

    position(int x,int y){
        first=x;
        second=y;
        normalize();
    }
    position(const position &p){
        first=p.first;
        second=p.second;
        normalize();
    }
    
    void normalize(){
        if(abs(first%2)!=abs(second%2)) first--;

    }

    vector<position> neighbor(){
        vector<position> v;
        v.emplace_back(first+1,second+1);
        v.emplace_back(first+2,second);
        v.emplace_back(first+1,second-1);
        v.emplace_back(first-1,second-1);
        v.emplace_back(first-2,second);
        v.emplace_back(first-1,second+1);
        return v;       
    }

    position applayMove(Direction d){
        position p(*this);
        switch (d)
        {
            case RIGHT:
                p.first+=2;
            break;
            case UP_RIGHT:
                p.first +=1;
                p.second-=1;
            break;
            case UP_LEFT:
                p.first -=1;
                p.second-=1;
            break;
            case DOWN_RIGHT:
                p.first +=1;
                p.second+=1;
            break;
            case DOWN_LEFT:
                p.first -=1;
                p.second+=1;
            break;
            case LEFT:
                p.first-=2;
            break;
        }
        p.normalize();
        return p;
    }
};


bool operator==(const position &p1,const position& p2){
    return (p1.first==p2.first && p1.second==p2.second);
}

bool operator!=(const position &p1,const position& p2){
    return !(p1.first==p2.first && p1.second==p2.second);
}

bool  isNear(const position &p1, const position &p2){
    if(p1.second==p2.second && abs(p1.first-p2.first)==2) return true;
    if(abs(p1.first-p2.first)<=1 && abs(p1.second-p2.second)<=1) return true;
    return false;
}


template<>
struct std::hash<position>
{
    std::size_t operator()(const position& p) const noexcept
    {
        return  hash<int>{}(p.first+1000*p.second);
    }
};

position NULL_POSITION= position(10000,10000);