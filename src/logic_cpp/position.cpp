#include <bits/stdc++.h>
#include "position.h"
#include "enums.h"
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



 |       |       |
 | -1,-1 | +1,-1 |
 |   UL  |   UR  |
  \     / \     /
   \   /   \   /
    \ /     \ /
     |       |
-2,0 |  0,0  | +2,0
 L   |       |   R
    / \     / \
   /   \   /   \
  /     \ /     \
 |       |       |
 | -1,+1 | +1,+1 |
    DL       DR


*/






position::position(int x,int y){
    first=x;
    second=y;
    normalize();
}
position::position(const position &p){
    first=p.first;
    second=p.second;
    normalize();
}
    
position::position(){
    first=10000;
    second=10000;
    normalize();
}
void position::normalize(){
    if(abs(first%2)!=abs(second%2)) first--;
}

vector<position> position::neighbor(){
    vector<position> v;
    v.emplace_back(first+1,second+1);
    v.emplace_back(first+2,second);
    v.emplace_back(first+1,second-1);
    v.emplace_back(first-1,second-1);
    v.emplace_back(first-2,second);
    v.emplace_back(first-1,second+1);
    return v;       
}

position position::applayMove(Direction d){
    pair<int,int> delta=associedDifference(d);
    position p(first+delta.first,second+delta.second);
    return p;
}



Direction getMovementDirection(position from, position to){
    pair<int,int> diff(to.first-from.first,to.second-from.second);

    if(diff.first==+2){
        return Direction::RIGHT;
    }
    if(diff.second==+1){
        if(diff.first==+1) return DOWN_RIGHT;
        return DOWN_LEFT;
    }
    if(diff.first==-2)
        return LEFT;
    if(diff.second==-1){
        if(diff.first==-1)return UP_LEFT;
        return UP_RIGHT;
    }
    return RIGHT;

}

pair<int,int> associedDifference(Direction d){
    return movementCircleClockwise[directionToNumber(d)];
}
int directionToNumber(Direction d){
    switch (d)
    {
        case RIGHT: return 0;
        case DOWN_RIGHT:return 1;
        case DOWN_LEFT:return   2;
        case LEFT:return   3;
        case UP_LEFT: return 4;
        case UP_RIGHT:return 5;
    }
    return 0;
}

Direction numberToDirection(int n){
    switch (n)
    {
        case 0:return RIGHT;
        case 1:return DOWN_RIGHT;
        case 2:return   DOWN_LEFT;
        case 3:return   LEFT;
        case 4:return UP_LEFT;
        case 5:return UP_RIGHT;
    }
    return RIGHT;
}

bool operator==(const position &p1,const position& p2){
    return (p1.first==p2.first && p1.second==p2.second);
}

bool operator!=(const position &p1,const position& p2){
    return !(p1.first==p2.first && p1.second==p2.second);
}

bool isNear(const position &p1, const position &p2){
    if(p1.second==p2.second && abs(p1.first-p2.first)==2) return true;
    if(abs(p1.first-p2.first)<=1 && abs(p1.second-p2.second)<=1) return true;
    return false;
}


vector<position> otherWay(position &p1,position &p2){
    Direction dir=getMovementDirection(p1,p2);
    int n=directionToNumber(dir);
    int m1=(n+1)%6;
    int m2=(n+5)%6;
    vector<position> ris;
    ris.push_back(p1.applayMove(numberToDirection(m1)));
    ris.push_back(p1.applayMove(numberToDirection(m2)));
    return ris;
}



position NULL_POSITION(10000,10000);

position HAND_POSITION(-10000,10000);

