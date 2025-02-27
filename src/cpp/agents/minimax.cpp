#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

#include <chrono>

int MinimaxAgent::utility(GameState state, const Board& board) {
    //TODO: implement a decent heuristic

    if (state == GameState::DRAW) {
        return 0;
    }

    if (color == PlayerColor::WHITE) {
        if (state == GameState::WHITE_WIN) {
            return 100;
        }
        if (state == GameState::BLACK_WIN) {
            return -100;
        }
        
        return 0;
    } else {
        if (state == GameState::BLACK_WIN) {
            return 100;
        }
        if (state == GameState::WHITE_WIN) {
            return -100;
        }
        
        return 0;
    }
}

action MinimaxAgent::initiate_minimax(Board& board) {

    int max_eval = MIN_EVAL;
    int alpha = MIN_EVAL;
    int beta = MAX_EVAL;
    action todo_action = pass();


    // Get all valid moves
    std::vector<action> valids = board.possibleMoves();

    if (!valids.empty()) {
        todo_action = valids[0];
    }

    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < valids.size(); i++) {
        // Play the move on the copy
        board.executeAction(ActionToString(valids[i]));

        
        // Try a simplified call first
        int eval = this->minmax(board.state, board, 1, alpha, beta);
        
        
        if (eval > max_eval) {
            max_eval = eval;
            todo_action = valids[i];
        }
        
        alpha = std::max(alpha, max_eval);

        board.undo(1);
    }
    
    return todo_action;
}

int MinimaxAgent::minmax(GameState state, Board& board, int depth, int alpha, int beta) {
    // Debug print
    
    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth >= depth_limit) {
        // Return a simple evaluation for debugging
        int eval = 0;
            // If Oracle fails, use utility as fallback
        eval = utility(state, board);
        return eval;
    }
    
    std::vector<action> valid_moves = board.possibleMoves();

    
        int max_eval = MIN_EVAL;
        
        for (const auto& action : valid_moves) {
                // Play the move
                board.executeAction(ActionToString(action));

                
                // Recursive call

                int eval = minmax(board.state, board, depth + 1, -beta, -alpha);
                max_eval = std::max(max_eval, eval);
                alpha = std::max(alpha, max_eval);

                board.undo(1);
                
                // Alpha-beta pruning
                if (beta <= alpha) {
                    break;
                }
        }
        
        return max_eval;
}

action MinimaxAgent::calculate_best_move(Board& board) {

    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();

    if (DISABLE_CACHE || _cache == INVALID_ACTION || board.currentTurn != cached_turn) {
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
    
        cout << "Time taken by function: "
             << duration.count()/1e6 << " seconds" << endl;
    
    return _cache;
}