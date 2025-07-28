#include "minimax_tt.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>
#include <bitset>
#include <unordered_map>


int MinimaxAgentTT::utility(GameState state, Board board) {
    if (state == GameState::DRAW) {
        return 0;
    }

    color = board.currentColor();

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
    return board.getScore(color);
}

//ITERATIVE DEEPENING WITH TRANSPOSITION TABLE:
actionT MinimaxAgentTT::initiate_minimax_iterative(Board board) {
    board.ComputePossibleMoves();

    if (board.numAction == 0) return pass();

    int best_eval = MIN_EVAL;
    actionT best_move = board.resAction[0];

    std::vector<actionT> actions(board.resAction, board.resAction + board.numAction);

    int current_depth = 3;

    while (!is_time_up()) {
        actionT move_this_depth = best_move;
        int max_eval = MIN_EVAL;
        int alpha = MIN_EVAL;
        int beta = MAX_EVAL;

        cout << "Current depth: " << current_depth << endl;

        // STEP 1: Move ordering using transposition table + shallow eval
        std::vector<std::pair<int, actionT>> scored_actions;
        
        for (const auto& action : actions) {
            if (is_time_up()) break;
            
            Board temp = board;
            temp.applayAction(action);
            
            // Try to get move ordering hint from transposition table
            uint64_t hash = temp.getHash(); // You'll need to implement this
            actionT tt_move = pass();
            int tt_value;
            
            if (tt.lookup(hash, 0, MIN_EVAL, MAX_EVAL, tt_value, tt_move)) {
                // Use transposition table value for ordering
                scored_actions.emplace_back(tt_value, action);
            } else {
                // Fall back to utility evaluation
                scored_actions.emplace_back(utility(temp.getGameState(), temp), action);
            }
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

            int eval = -minmax(b1.getGameState(), b1, current_depth, -beta, -alpha);
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

int MinimaxAgentTT::minmax(GameState state, Board board, int depth_remaining, int alpha, int beta) {
    if (is_time_up()) {
        return 0;  // Return a neutral evaluation when time runs out
    }

    cout << "Getting hash" << endl;
    // Get board hash for transposition table
    uint64_t hash = board.getHash(); // You need to implement this in Board class
    cout << "Got hash" << endl;
    
    // Check transposition table
    actionT tt_move = pass();
    int tt_value;
    if (tt.lookup(hash, depth_remaining, alpha, beta, tt_value, tt_move)) {
        return tt_value;
    }

    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth_remaining == 0) {

        int value = utility(state, board);
        // Store in transposition table
        tt.store(hash, value, depth_remaining, TT_EXACT, pass());
        return value;
    }
    
    board.ComputePossibleMoves();
    
    int max_eval = MIN_EVAL;
    actionT best_move = pass();
    bool found_pv = false; // Principal variation found
    
    // Try the transposition table move first if available
    std::vector<actionT> ordered_moves;
    bool tt_move_valid = false;
    
    // Check if tt_move is in the legal moves
    for (int i = 0; i < board.numAction; i++) {
        if (board.resAction[i] == tt_move) {
            ordered_moves.push_back(tt_move);
            tt_move_valid = true;
            break;
        }
    }
    
    // Add remaining moves
    for (int i = 0; i < board.numAction; i++) {
        if (!tt_move_valid || board.resAction[i] != tt_move) {
            ordered_moves.push_back(board.resAction[i]);
        }
    }
    
    int original_alpha = alpha;
    
    for (const auto& move : ordered_moves) {
        Board b1 = Board(board);
        b1.applayAction(move);

        int eval = -minmax(b1.getGameState(), b1, depth_remaining - 1, -beta, -alpha);
        
        if (eval > max_eval) {
            max_eval = eval;
            best_move = move;
        }
        
        alpha = std::max(alpha, max_eval);

        // Alpha-beta pruning
        if (beta <= alpha) {
            break;
        }
    }
    
    // Store in transposition table
    int flag;
    if (max_eval <= original_alpha) {
        flag = TT_UPPERBOUND; // All moves were worse than alpha
    } else if (max_eval >= beta) {
        flag = TT_LOWERBOUND; // We found a move that caused beta cutoff
    } else {
        flag = TT_EXACT; // We have an exact value
    }
    
    tt.store(hash, max_eval, depth_remaining, flag, best_move);
        
    return max_eval;
}

actionT MinimaxAgentTT::calculate_best_move(Board board) {
    start_time = std::chrono::high_resolution_clock::now();

    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();
    calledBoard = 0;

    // Initial moves
    if (board.currentTurn <= 2) {
        return board.suggestInitialMove();
    }

    if (DISABLE_CACHE || _cache == pass() || board.currentTurn != cached_turn) {
        color = board.currentColor();
        cached_turn = board.currentTurn;
        
        // Clear transposition table for new search
        // (Optional: you might want to keep it across moves)
        // tt.clear();
        
        _cache = initiate_minimax_iterative(board);
    }

    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    cout << "TT size: " << tt.size() << " entries" << endl;
    
    return _cache;
}