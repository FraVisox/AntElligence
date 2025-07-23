#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>

int MinimaxAgent::utility(GameState state, EBoard* board) {
    

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
    return board->getScore(color);

    return 0;
}

string MinimaxAgent::initiate_minimax(EBoard* board) {
    int max_eval = MIN_EVAL;
    int alpha = MIN_EVAL;
    int beta = MAX_EVAL;
    actionT todo_action = 2000;

    actionT* valids = new actionT[256];

    // Get all valid moves
    board->getNextsActions(valids);

    // TODO: how to do this?
    if (!valids.empty()) {
        todo_action = valids[0];
    }
    if (valids.size() == 1) {
        return todo_action;
    }

    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < valids.size(); i++) {
        // Play the move on the copy
        board->applyAction(valids[i]);

        // Try a simplified call first
        int eval = this->minmax(board, board, 1, alpha, beta);
        
        
        if (eval > max_eval) {
            max_eval = eval;
            todo_action = valids[i];
        }
        
        alpha = std::max(alpha, max_eval);

        board.undo(1);

    }
    
    return ActionToString(todo_action);
}

int MinimaxAgent::minmax(GameState state, EBoard* board, int depth, int alpha, int beta) {

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
    
    actionT* valids = new actionT[256];
    board->getNextsActions(valid_moves);

    
    int max_eval = MIN_EVAL;
    
    for (const auto& action : valid_moves) {
        // Play the move
        board.executeAction(ActionToString(action));
        
        for (const auto& action : valid_moves) {
                // Play the move
                board.executeActionUnsafe(action);
        }

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


/*
        STARTING TIPS:

        WHITE:

        Turn 1: Place wG1

        Turn 3: Place wQ to form elbow:
        - If opponent is on your right → place Queen on left of wG1
        - If opponent is on your left → place Queen on right of wG1

        Turn 5: Place wA1 toward where you expect enemy Queen

        BLACK:

        Turn 2: Place bS1 adjacent to wG1 (only one legal choice)

        Turn 4: Place bQ next to bS1 (form inline or elbow shape)
            - Prefer inline (bQ opposite wG1 from bS1), unless blocked

        Turn 6: Add bA1 near bQ
        
        */

string MinimaxAgent::calculate_best_move(EBoard* board) {

    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();
    calledBoard=0;

    // Initial moves
    int cached_turn_new = board->getTurn();
    if (cached_turn_new <= 4) {

        if (cached_turn_new == 1) {
            return "wG1";
        } else if (cached_turn_new == 2) {
            return "wS1 -"+board.firstPiece();
        // TODO: understand what to do here
        } else if (cached_turn_new == 3) {
            return "wQ -"+board.firstPiece();
        } else {
            return "wA1 -"+board.firstPiece();
        }
    }

    if (DISABLE_CACHE || _cache == "" || cached_turn_new != cached_turn) {
        cached_turn = cached_turn_new;
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