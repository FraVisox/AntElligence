#ifndef MINIMAX_H
#define MINIMAX_H

#include "engine/board.h"
#include "engine/enums.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>


class MinimaxAgent {
private:
    int calledBoard;
    PlayerColor color;
    int toale_evaled;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    const std::chrono::seconds TIME_LIMIT = std::chrono::seconds(5);

    actionT _cache = pass();
    int cached_turn = 1;
    bool DISABLE_CACHE = false;

    std::chrono::seconds timeLimit;
    int depthLimit = 4;

    const int MIN_EVAL = -1000000;
    const int MAX_EVAL = 1000000;

    int utility(GameState state, Board &board);
    actionT initiate_minimax_iterative(Board &board);
    actionT initiate_minimax_fixed(Board board);
    int minmax(GameState state, Board& board, int depth, int alpha, int beta);

    bool is_time_up() const {
        return std::chrono::high_resolution_clock::now() - start_time >= timeLimit;
    }

public:
    MinimaxAgent(int tl = -1, int dl = 5) : color(PlayerColor::WHITE) {
        if(tl==-1)
            timeLimit=TIME_LIMIT;
        else
            timeLimit = chrono::seconds(tl);
        depthLimit = dl;
    }
    int parallel_minmax(Board &board, int depth_remaining, int alpha, int beta);

    actionT calculate_best_move(Board &board);
    actionT initiate_minimax_iterative_parallel(Board &board);
};

#endif