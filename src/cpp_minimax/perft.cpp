#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <string>
#include "engine/board.h"
#include "engine/enums.h"

// ---------------------------------------------------------------------------
// Recursive perft kernel
//   depth 0 → count this node (return 1)
//   depth k → sum perft(child, k-1) over all legal moves
//   Terminal positions (game over) have no children, so they contribute 0
//   for k > 0, which is correct: the game ended before we reached a leaf.
// ---------------------------------------------------------------------------
static uint64_t perft_inner(Board& board, int depth) {
    if (depth == 0) return 1;

    GameState state = board.getGameState();
    if (state == WHITE_WIN || state == BLACK_WIN || state == DRAW) return 0;

    board.ComputePossibleMoves();
    int n = board.numAction;
    if (n == 0) return 0;

    // Snapshot actions – recursive call overwrites board.resAction.
    actionT actions[MAX_ACTIONS_SIZE];
    memcpy(actions, board.resAction, n * sizeof(actionT));

    uint64_t count = 0;
    for (int i = 0; i < n; i++) {
        board.applayAction(actions[i]);
        count += perft_inner(board, depth - 1);
        board.undoAction();
    }
    return count;
}

// ---------------------------------------------------------------------------
// Parallel perft: parallelise the top-level move loop with OpenMP.
// Each thread works on its own Board copy to avoid data races.
// ---------------------------------------------------------------------------
static uint64_t perft_parallel(Board& board, int depth) {
    if (depth == 0) return 1;

    GameState state = board.getGameState();
    if (state == WHITE_WIN || state == BLACK_WIN || state == DRAW) return 0;

    board.ComputePossibleMoves();
    int n = board.numAction;
    if (n == 0) return 0;

    actionT actions[MAX_ACTIONS_SIZE];
    memcpy(actions, board.resAction, n * sizeof(actionT));

    uint64_t count = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+:count) schedule(dynamic, 1)
#endif
    for (int i = 0; i < n; i++) {
        Board local = board;              // per-thread copy
        local.applayAction(actions[i]);
        count += perft_inner(local, depth - 1);
    }
    return count;
}

// ---------------------------------------------------------------------------
// Top-level perft: iterates depths 1..max_depth and prints timing, mirroring
// the output style of nokamute's `minimax::perft`.
// ---------------------------------------------------------------------------
static void perft(Board& board, int max_depth, bool parallel) {
    std::cout << "depth 0: 1\n";
    for (int d = 1; d <= max_depth; d++) {
        auto t0 = std::chrono::steady_clock::now();

        uint64_t nodes = parallel ? perft_parallel(board, d)
                                  : perft_inner(board, d);

        auto t1 = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        std::cout << "depth " << d << ": " << nodes
                  << " (" << ms << " ms)\n";
        std::cout.flush();
    }
}

// ---------------------------------------------------------------------------
// CLI: perft [GameType] [max_depth] [--parallel]
//   GameType : Base | Base+M | Base+L | Base+P | Base+ML | Base+MP |
//              Base+LP | Base+MLP   (default: Base)
//   max_depth: integer                (default: 6)
//   --parallel: use OpenMP parallelism (default: off)
// ---------------------------------------------------------------------------
static GameType parseGameType(const std::string& s) {
    if (s == "Base")     return GameType::Base;
    if (s == "Base+M")   return GameType::Base_M;
    if (s == "Base+L")   return GameType::Base_L;
    if (s == "Base+P")   return GameType::Base_P;
    if (s == "Base+ML")  return GameType::Base_ML;
    if (s == "Base+MP")  return GameType::Base_MP;
    if (s == "Base+LP")  return GameType::Base_LP;
    if (s == "Base+MLP") return GameType::Base_MLP;
    std::cerr << "Unknown game type '" << s << "', using Base\n";
    return GameType::Base;
}

int main(int argc, char* argv[]) {
    std::string game_type_str = "Base";
    int max_depth = 6;
    bool parallel = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--parallel") {
            parallel = true;
        } else if (arg[0] == '-' && arg.size() > 1 && std::isdigit(arg[1])) {
            // allow negative depth? ignore
        } else if (std::isdigit(arg[0])) {
            max_depth = std::stoi(arg);
        } else {
            game_type_str = arg;
        }
    }

    GameType gt = parseGameType(game_type_str);
    Board board(gt);

    std::cout << game_type_str << "\n";
    perft(board, max_depth, parallel);
    return 0;
}
