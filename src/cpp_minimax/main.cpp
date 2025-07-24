#include <iostream>
#include "engine.h"
#include "agents/random.h"
#include "agents/minimax.h"

#include "engine/action.cpp"
#include "engine/board.cpp"
#include "engine/direction.cpp"
#include "engine/engine_interface.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "agents/minimax.cpp"
#include "engine.cpp"

int main(){

    MinimaxAgent a;

    Engine e(a);

    e.start();
    return 0;
}