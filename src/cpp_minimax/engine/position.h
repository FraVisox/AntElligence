#ifndef POSITION_H
#define POSITION_H
#include <string>
#include <vector>
#include <cstdint>
#include "enums.h"
#include "direction.h"
#define SIZE_BOARD 32


typedef uint16_t positionT;

const positionT applayMove(const positionT pos,const direction d);






#endif