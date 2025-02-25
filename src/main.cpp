#include <iostream>
#include "engine_cpp/engine.h"
#include "agents_cpp/random.h"

#include "engine_cpp/action.cpp"
#include "engine_cpp/board.cpp"
#include "engine_cpp/direction.cpp"
#include "engine_cpp/engine.cpp"
#include "engine_cpp/enums.cpp"
#include "engine_cpp/gameboard.cpp"
#include "engine_cpp/piece.cpp"
#include "engine_cpp/position.cpp"

int main(){

    RandomAgent a;

    Engine e(a);

    e.start();
    return 0;
}