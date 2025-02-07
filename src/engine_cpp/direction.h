#ifndef DIRECTION_H
#define DIRECTION_H
#include <utility>
using namespace std;
#include <string>

enum direction{
    RIGHT=0,
    DOWN_RIGHT=1,
    DOWN_LEFT=2,
    LEFT=3,
    UP_LEFT=4,
    UP_RIGHT=5,
    OVER=6,
    INVALID = -1
};

//Vectors of directions

extern direction allDirections[6];
extern pair<int,int> movementCircleClockwise[7];

//Functions

string nameDirToString(string name, direction dir);
direction numberToDirection(int n);
direction oppositeDir(direction d);
pair<int,int> associatedDifference(direction d);
direction extractDirection(string s);

#endif