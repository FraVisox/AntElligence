#ifndef MINIMAX_H
#define MINIMAX_H

#include "cpp_minimax/cpp/engine/embadded_board.h"
#include <vector>
#include <algorithm>
#include <memory>

class MinimaxAgent {
private:
    int calledBoard;
    PlayerColor color;
    string _cache = "";
    int cached_turn = 1;
    bool DISABLE_CACHE = true;

    int timeLimit = 5;
    int depthLimit = 5;

    const int MIN_EVAL = -10000;
    const int MAX_EVAL = 10000;

    int utility(GameState state, EBoard* board);
    string initiate_minimax(EBoard* board);
    int minmax(GameState state, EBoard* board, int depth, int alpha, int beta);

public:
    MinimaxAgent(int tl = 100000, int dl = 5) : color(PlayerColor::WHITE) {
        timeLimit = tl;
        depthLimit = dl;
    }

    string calculate_best_move(EBoard* board);
};

#endif