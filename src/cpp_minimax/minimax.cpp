#include "minimax.h"
#include <algorithm>
#include <atomic>
#include <bitset>
#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "TT.h"
#include <omp.h>

bool MinimaxAgent::is_time_up() const {
    return (std::chrono::high_resolution_clock::now() - start_time) >= timeLimit;
}
std::atomic<long> toale_evaled{0};


int SETUP_depth_pruning = 3;

struct MoveScore {
    actionT move{};
    double  score{};
    bool operator<(const MoveScore& rhs) const noexcept { return score < rhs.score; }
};

double MinimaxAgent::utility(Board &board) const {
    toale_evaled.fetch_add(1, std::memory_order_relaxed);
        return ev.evalBoardCurrentPlayer(board);
}

// Negamax with alpha-beta pruning. Keep signature/name for compatibility.
double MinimaxAgent::minmax2(Board& board, int depth_remaining, double alpha, double beta) const {
    if (is_time_up()) return MAX_EVAL; // sentinel: caller should stop searching

    // Base cases
    GameState state = board.getGameState();
    if (state == GameState::DRAW    ||
        state == GameState::WHITE_WIN||
        state == GameState::BLACK_WIN||
        depth_remaining == 0) {
        return utility(board);
    }

    board.ComputePossibleMoves();
    const int n = board.numAction;
    vector<actionT> nextActs(n);
    
    
    for(int i=0;i<n;i++) nextActs[i]=board.resAction[i];

    // Fast leaf (no parallel here)
    if (depth_remaining == 1) {
        double best_value = MIN_EVAL;
        for (int i = 0; i < n; ++i) {

            const actionT mv = nextActs[i];
            board.applayAction(mv);
            // negamax: value = -eval(child)
            const double v = -utility(board);
            board.undoAction();
            if (v > best_value) best_value = v;
            if (best_value > alpha) alpha = best_value;
            if (alpha >= beta) break;
        }
        return best_value;
    }

    // Build a small local buffer for ordering
    std::vector<MoveScore> ordered;
    ordered.reserve(n);
    for (int i = 0; i < n; ++i) {
        const actionT mv = nextActs[i];
        board.applayAction(mv);
        ordered.push_back(MoveScore{mv, utility(board)});
        board.undoAction();
    }
    std::sort(ordered.begin(), ordered.end()); // low->high; we use negamax so high is searched later

    double best_value = MIN_EVAL;

    // Parallel only at the chosen frontier (same as your SETUP_depth_pruning)
    if (depth_remaining == SETUP_depth_pruning) {
        std::atomic<bool> exitAll{false};
        std::atomic<double> alphaAtomic{alpha};

        // Results buffer
        std::vector<double> resUtil(n, MIN_EVAL);

        #pragma omp parallel for schedule(dynamic,1)
        for (int i = 0; i < n; ++i) {
            if (exitAll.load(std::memory_order_acquire)) continue;

            // Copy board at this split point (your Board copy ctor by move is used elsewhere; follow that)
            Board b1(board, ordered[i].move);
            b1.ComputePossibleMoves();
            const int innAct = b1.numAction;

            // Second-level ordering (cheap)
            vector<actionT> innNext(innAct);
            for(int i=0;i<innAct;++i)innNext[i]=b1.resAction[i];
            std::vector<MoveScore> inner;
            inner.reserve(innAct);
            for (int q = 0; q < innAct; ++q) {
                const actionT mv = innNext[q];
                b1.applayAction(mv);
                inner.push_back(MoveScore{mv, utility(b1)});
                b1.undoAction();
            }
            std::sort(inner.begin(), inner.end());

            // Local window using a snapshot of global alpha
            double localAlpha = -beta;                 // note: negated window for child (negamax)
            double localBeta  = -alphaAtomic.load(std::memory_order_acquire);
            double best_local = MIN_EVAL;

            for (int q = 0; q < innAct; ++q) {
                if (exitAll.load(std::memory_order_acquire) || is_time_up()) break;

                const actionT mv = inner[q].move;
                b1.applayAction(mv);
                // jump two plies: we already applied one ply (root child), this is the second
                const double score = -minmax2(b1, depth_remaining - 2, -localBeta, -localAlpha);
                b1.undoAction();

                if (score > best_local) best_local = score;
                if (best_local > localAlpha) localAlpha = best_local;
                if (localAlpha >= localBeta) break;

                // tighten local beta from global alpha (reduces work as global alpha rises)
                const double snapA = alphaAtomic.load(std::memory_order_acquire);
                if (-snapA < localBeta) localBeta = -snapA;
            }

            resUtil[i] = -best_local; // back to parent perspective

            // Try to raise global alpha if this move improved
            double curA = alphaAtomic.load(std::memory_order_relaxed);
            while (resUtil[i] > curA && !alphaAtomic.compare_exchange_weak(curA, resUtil[i],
                                                                           std::memory_order_release,
                                                                           std::memory_order_relaxed)) {
                // curA updated with current value; loop until success/fail
            }
            // Cutoff?
            if (alphaAtomic.load(std::memory_order_acquire) >= beta) {
                exitAll.store(true, std::memory_order_release);
            }
        }

        for (int i = 0; i < n; ++i) {
            if (resUtil[i] > best_value) best_value = resUtil[i];
            if (best_value > alpha) alpha = best_value;
            if (alpha >= beta) break;
        }
        return best_value;
    }

    // Regular (non-parallel) search
    for (int i = 0; i < n; ++i) {
        const actionT mv = ordered[i].move;
        board.applayAction(mv);
        const double v = -minmax2(board, depth_remaining - 1, -beta, -alpha);
        board.undoAction();

        if (v > best_value) best_value = v;
        if (best_value > alpha) alpha = best_value;
        if (alpha >= beta) break;

        if (is_time_up()) break;
    }
    return best_value;
}

actionT MinimaxAgent::initiate_minimax_parallel(Board& board, int depth) {
    // Terminal? (defensive; also keeps your early exit)
    const GameState st = board.getGameState();
    if (st == GameState::DRAW || st == GameState::WHITE_WIN || st == GameState::BLACK_WIN) {
        return pass();
    }

    double best_value = MIN_EVAL;
    double alpha = MIN_EVAL, beta = MAX_EVAL;

    board.ComputePossibleMoves();
    const int numAction = board.numAction;
    if (numAction == 0) return pass();

    // Root move ordering
    std::vector<MoveScore> ordered;
    ordered.reserve(numAction);
    for (int i = 0; i < numAction; ++i) {
        const actionT mv = board.resAction[i];
        board.applayAction(mv);
        ordered.push_back(MoveScore{mv, utility(board)});
        board.undoAction();
    }
    std::sort(ordered.begin(), ordered.end());

    actionT best_move = ordered[0].move;

    for (int j = 0; j < numAction; ++j) {
        if (is_time_up()) break;

        const actionT move = ordered[j].move;
        board.applayAction(move);
        const double eval = -minmax2(board, depth - 1, -beta, -alpha);
        board.undoAction();

        if (eval > best_value) {
            best_value = eval;
            best_move  = move;
        }
        if (eval > alpha) alpha = eval;
        if (alpha >= beta) break;
    }

    std::clog << "Evaluated " << toale_evaled.load() << " boards, best " << best_move
              << " value " << best_value << " depth " << depth << std::endl;
    return best_move;
}

actionT MinimaxAgent::calculate_best_move(Board& board) {
    toale_evaled.store(0, std::memory_order_relaxed);
    start_time = std::chrono::high_resolution_clock::now();

    if (board.currentTurn <= 2) {
        std::clog << "Suggested";
        return board.suggestInitialMove();
    }


    actionT next_move;
    int depth = 4;
    while (!is_time_up() && depth < 15) {
        toale_evaled.store(0, std::memory_order_relaxed);
        // Keep your split depth; you can tune it per branching factor
        SETUP_depth_pruning = 3;

        const actionT candidate = initiate_minimax_parallel(board, depth);
        if (is_time_up()) break;

        next_move = candidate;
        ++depth;
    }

    // Your HMP cleanup left intact (if you re-enable it)
    //HMP[board.currentTurn % 10].clear();
    //HMP[(board.currentTurn + 1) % 10].clear();

    return next_move;
}
