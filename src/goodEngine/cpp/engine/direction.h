#ifndef DIRECTION_H
#define DIRECTION_H
using namespace std;
#include <string>


typedef uint_fast16_t direction;

extern const direction RIGHT;
extern const direction DOWN_RIGHT;
extern const direction DOWN_LEFT;
extern const direction LEFT;
extern const direction UP_LEFT;
extern const direction UP_RIGHT;
extern const direction OVER;
//extern const direction INVALID;


//Vectors of directions

//Functions

string nameDirToString(string name, direction dir);
direction oppositeDir(direction d);
const extern unsigned int dirDif[];
#endif