#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>
#include <bitset>

int MinimaxAgent::utility(GameState state, Board board) {
    if (state == GameState::DRAW) {
        return 0;
    }

    if (color == PlayerColor::WHITE) {
        if (state == GameState::WHITE_WIN) {
            return MAX_EVAL;
        }
        if (state == GameState::BLACK_WIN) {
            return MIN_EVAL;
        }
    } else {
        if (state == GameState::BLACK_WIN) {
            return MAX_EVAL;
        }
        if (state == GameState::WHITE_WIN) {
            return MIN_EVAL;
        }
    }
    //return board.getScore(color);
    return 0;
}

//ITERATIVE DEEPENING:
actionT MinimaxAgent::initiate_minimax(Board board) {
    actionT best_move = pass();
    int best_eval = MIN_EVAL;

    board.ComputePossibleMoves();
    if (board.numAction == 0) return pass();
    best_move = board.resAction[0];

    std::vector<actionT> actions(board.resAction, board.resAction + board.numAction);

    int current_depth = 5;

    while (!is_time_up()) {
        actionT move_this_depth = best_move;
        int max_eval = MIN_EVAL;
        int alpha = MIN_EVAL;
        int beta = MAX_EVAL;

        cout << "Current depth: " << current_depth << endl;

        // STEP 1: Move ordering – shallow eval at depth 1
        std::vector<std::pair<int, actionT>> scored_actions;
        for (const auto& action : actions) {
            if (is_time_up()) break;
            Board temp = board;
            temp.applayAction(action);
            int eval = minmax(temp.getGameState(), temp, 1, alpha, beta); // depth=1 eval
            scored_actions.emplace_back(eval, action);
        }

        // Sort actions descending by score (most promising first)
        std::sort(scored_actions.begin(), scored_actions.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });

        // Replace actions list with sorted version
        actions.clear();
        for (const auto& pair : scored_actions) {
            actions.push_back(pair.second);
        }

        // STEP 2: Do actual search at current depth
        for (const auto& action : actions) {
            if (is_time_up()) break;

            Board b1 = board;
            b1.applayAction(action);

            int eval = minmax(b1.getGameState(), b1, current_depth, alpha, beta);
            if (eval > max_eval) {
                max_eval = eval;
                move_this_depth = action;
            }

            alpha = std::max(alpha, max_eval);
        }

        // If time was not up, accept this result
        if (!is_time_up()) {
            best_move = move_this_depth;
            best_eval = max_eval;
        }

        current_depth++; // Increase search depth for next round
    }

    return best_move;
}

/*
// TRANSPOSITION TABLE
int MinimaxAgent::minmax(GameState state, Board board, int depth_remaining, int alpha, int beta) {
    calledBoard++;

    if (is_time_up()) return 0;

    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth_remaining == 0) {
        return utility(state, board);
    }

    bitset<308> hash = board.simple_hash(color);

    // Transposition Table Lookup
    auto it = transposition_table.find(hash);
    if (it != transposition_table.end()) {
        const TTEntry& entry = it->second;

        if (entry.bound == BoundType::EXACT) return entry.value;
        if (entry.bound == BoundType::LOWER_BOUND && entry.value >= beta) return entry.value;
        if (entry.bound == BoundType::UPPER_BOUND && entry.value <= alpha) return entry.value;
    }

    board.ComputePossibleMoves();
    int max_eval = MIN_EVAL;
    int original_alpha = alpha;

    for (int i = 0; i < board.numAction; i++) {
        if (is_time_up()) break;

        Board b1 = Board(board);
        b1.applayAction(board.resAction[i]);

        int eval = -minmax(b1.getGameState(), b1, depth_remaining - 1, -beta, -alpha);
        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        if (beta <= alpha) break;
    }

    // Transposition Table Store
    BoundType bound;
    if (max_eval <= original_alpha) bound = BoundType::UPPER_BOUND;
    else if (max_eval >= beta) bound = BoundType::LOWER_BOUND;
    else bound = BoundType::EXACT;

    transposition_table[hash] = { max_eval, bound };

    return max_eval;
}
    */


//FIXED DEPTH:
/*
actionT MinimaxAgent::initiate_minimax(Board board) {
    int max_eval = MIN_EVAL;
    int alpha = MIN_EVAL;
    int beta = MAX_EVAL;
    actionT todo_action = pass();


    // Get all valid moves
    board.ComputePossibleMoves();

    if (board.numAction != 0) {
        todo_action = board.resAction[0];
    }
    if (board.numAction == 1) {
        return todo_action;
    }

    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < board.numAction; i++) {
        // Play the move on the copy
        Board b1 = Board(board);
        b1.applayAction(board.resAction[i]);

        // Try a simplified call first
        int eval = this->minmax(b1.getGameState(), b1, 1, alpha, beta);
        
        if (eval > max_eval) {
            max_eval = eval;
            todo_action = board.resAction[i];
        }
        
        alpha = std::max(alpha, max_eval);

    }
    
    return todo_action;
}
    */

int MinimaxAgent::minmax(GameState state, Board board, int depth_remaining, int alpha, int beta) {

    // Debug print
    calledBoard++;

    if (is_time_up()) {
        return 0;  // Return a neutral evaluation when time runs out
    }


    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth_remaining == 0) {
        // Return a simple evaluation for debugging
        int eval = 0;
            // If Oracle fails, use utility as fallback
        eval = utility(state, board);
        return eval;
    }
    
    board.ComputePossibleMoves();

    
    int max_eval = MIN_EVAL;
    
    for (int i = 0; i < board.numAction; i++) {

        Board b1 = Board(board);
        b1.applayAction(board.resAction[i]);

        int eval = -minmax(b1.getGameState(), b1, depth_remaining - 1, -beta, -alpha);
        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        // Alpha-beta pruning
        if (beta <= alpha) {
            break;
        }

    }
        
    return max_eval;
}

actionT MinimaxAgent::calculate_best_move(Board board) {

    start_time = std::chrono::high_resolution_clock::now();

    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();
    calledBoard=0;

    // Initial moves
    if (board.currentTurn <= 2) {
        return board.suggestInitialMove();
    }

    if (DISABLE_CACHE || _cache == pass() || board.currentTurn != cached_turn) {
        color = board.currentColor();
        cached_turn = board.currentTurn;
        _cache = initiate_minimax(board);
    }


    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();

    // Get duration. Substart timepoints to 
    // get duration. To cast it to proper unit
    // use duration cast method
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    //cout << "Time taken by function: " << duration.count()/1e6 << " seconds with " <<calledBoard << " evaluation" << endl;
    
    return _cache;
}