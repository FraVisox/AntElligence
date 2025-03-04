#ifndef MINIMAX_H
#define MINIMAX_H

#include "agent.h"
#include <vector>
#include <algorithm>
#include <memory>

class MinimaxAgent : public Agent {
private:
    int calledBoard;
    PlayerColor color;
    action _cache = INVALID_ACTION;
    int cached_turn = 1;
    bool DISABLE_CACHE = true;

    const int MIN_EVAL = -10000;
    const int MAX_EVAL = 10000;

    int utility(GameState state, Board& board);
    action initiate_minimax(Board& board);
    int minmax(GameState state, Board& board, int depth, int alpha, int beta);

public:
    MinimaxAgent(int tl = 100000, int dl = 7) : Agent(tl, dl), color(PlayerColor::WHITE) {}

    action calculate_best_move(Board& board) override;
};

#endif