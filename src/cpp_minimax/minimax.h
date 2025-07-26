#ifndef MINIMAX_H
#define MINIMAX_H

#include "engine/board.h"
#include "engine/enums.h"
#include <vector>
#include <algorithm>
#include <memory>

class MinimaxAgent {
private:
    int calledBoard;
    PlayerColor color;
    actionT _cache = pass();
    int cached_turn = 1;
    bool DISABLE_CACHE = true;

    int timeLimit = 5;
    int depthLimit = 5;

    const int MIN_EVAL = -10000;
    const int MAX_EVAL = 10000;

    int utility(GameState state, Board board);
    actionT initiate_minimax(Board board);
    int minmax(GameState state, Board board, int depth, int alpha, int beta);

public:
    MinimaxAgent(int tl = 100000, int dl = 5) : color(PlayerColor::WHITE) {
        timeLimit = tl;
        depthLimit = dl;
    }

    actionT calculate_best_move(Board board);
};

#endif