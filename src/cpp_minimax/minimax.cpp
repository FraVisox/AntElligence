#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>

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

        // TODO: undo
    }
    
    return todo_action;
}

int MinimaxAgent::minmax(GameState state, Board board, int depth, int alpha, int beta) {

    // Debug print
    calledBoard++;
    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth >= depthLimit) {
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

        int eval = minmax(b1.getGameState(), b1, depth + 1, -beta, -alpha);
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

    cout << "Time taken by function: " << duration.count()/1e6 << " seconds with " <<calledBoard << "evaluation" << endl;
    
    return _cache;
}