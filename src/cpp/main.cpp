#include "engine.h"
#include "agents/random.h"
#include "agents/minimax.h"

//Compilation without the following includes: g++ main.cpp engine.cpp agents/*.cpp engine/*.cpp -O3 -o main

#include "engine/action.cpp"
#include "engine/board.cpp"
#include "engine/direction.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "agents/minimax.cpp"
#include "engine.cpp"

int main(){

    //RandomAgent a;
    MinimaxAgent a;

    Engine e(a);

    e.start();
    return 0;
}