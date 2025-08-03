#ifndef MINIMAX_H
#define MINIMAX_H

#include "engine/board.h"
#include "engine/enums.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>
#include  "engine/dynamic_eval.h"
#include <unordered_map>
class MinimaxAgent {
    DynEval ev;
public:
    int calledBoard;
    PlayerColor color;
    int toale_evaled=0;
    int toale_evaled2=0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    const std::chrono::seconds TIME_LIMIT = std::chrono::seconds(5);
    
    actionT _cache = pass();
    int cached_turn = 1;
    bool DISABLE_CACHE = false;

    std::chrono::seconds timeLimit;

    const double MIN_EVAL = -1e10;
    const double MAX_EVAL = 1e10;

    double utility(Board &board);
    double utility2(Board &board);
    actionT initiate_minimax_fixed(Board &board,int depth);
    double minmax(Board& board, int depth, double alpha, double beta);

    bool is_time_up() const;

    MinimaxAgent(int tl = -1) : color(PlayerColor::WHITE) {
        if(tl==-1)
            timeLimit=TIME_LIMIT;
        else
            timeLimit = chrono::seconds(tl);
    }

    double parallel_minimax_first(Board board, int depth_remaining, double alpha, double beta);
    double parallel_minmax(Board &board, int depth_remaining, double alpha, double beta);

    actionT calculate_best_move(Board &board);
    actionT initiate_minimax_parallel(Board &board,int depth);
};





#endif