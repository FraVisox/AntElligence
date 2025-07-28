#include <iostream>
#include "engine.h"
#include "minimax.h"
#include "minimax_tt.h"
#include "minimax.h"

#include "engine/action.cpp"
#include "engine/board_bitset.cpp"
#include "engine/board.cpp"
#include "engine/board_score.cpp"
#include "engine/board_score_mzinga.cpp"
#include "engine/direction.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "minimax.cpp"
#include "minimax_tt.cpp"
#include "engine.cpp"

int main(){

    MinimaxAgent a;

    Engine e(a);

    e.start();
    return 0;
}