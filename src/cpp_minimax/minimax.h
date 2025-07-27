#ifndef MINIMAX_H
#define MINIMAX_H

#include "engine/board.h"
#include "engine/enums.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>

#include <unordered_map>

enum class BoundType { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    int value;
    int depth;
    BoundType bound;
};

std::unordered_map<bitset<308>, TTEntry> transposition_table;


class MinimaxAgent {
private:
    int calledBoard;
    PlayerColor color;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    const std::chrono::seconds TIME_LIMIT = std::chrono::seconds(5);

    actionT _cache = pass();
    int cached_turn = 1;
    bool DISABLE_CACHE = true;

    int timeLimit = 5;
    int depthLimit = 15;

    const int MIN_EVAL = -10000;
    const int MAX_EVAL = 10000;

    int utility(GameState state, Board board);
    actionT initiate_minimax(Board board);
    int minmax(GameState state, Board board, int depth, int alpha, int beta);

    bool is_time_up() const {
        return std::chrono::high_resolution_clock::now() - start_time >= TIME_LIMIT;
    }

public:
    MinimaxAgent(int tl = 100000, int dl = 5) : color(PlayerColor::WHITE) {
        timeLimit = tl;
        depthLimit = dl;
    }

    actionT calculate_best_move(Board board);
};

#endif