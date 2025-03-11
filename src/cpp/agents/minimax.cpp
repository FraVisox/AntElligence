#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

//https://github.com/jonthysell/Mzinga/blob/main/src/Mzinga/Core/AI/GameAI.cs


/*
//Noisy means that it moves near to opponent queen
//Quiet means everything else


score += colorValue * metricWeights.Get(bugType, BugTypeWeight.InPlayWeight) * boardMetrics[pieceName].InPlay; //inplay is 0 or 1
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.IsPinnedWeight) * boardMetrics[pieceName].IsPinned; //ispinned is 0 or 1
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.IsCoveredWeight) * boardMetrics[pieceName].IsCovered; //iscovered is 0 or 1
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.NoisyMoveWeight) * boardMetrics[pieceName].NoisyMoveCount; //how many moves he has that are "noisy"
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.QuietMoveWeight) * boardMetrics[pieceName].QuietMoveCount; //how many moves he has that are "quiet"
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.FriendlyNeighborWeight) * boardMetrics[pieceName].FriendlyNeighborCount; //how many friendly neighbors
score += colorValue * metricWeights.Get(bugType, BugTypeWeight.EnemyNeighborWeight) * boardMetrics[pieceName].EnemyNeighborCount; //how many enemy neighbors

If I have not placed pieces
// Pieces still in hand, blend start and end scores
double startScore = CalculateBoardScore(boardMetrics, startMetricWeights);

double startRatio = boardMetrics.PiecesInHand / (double)(boardMetrics.PiecesInHand + boardMetrics.PiecesInPlay);
double endRatio = 1 - startRatio;

return (startRatio * startScore) + (endRatio * endScore);


TODO: se alleniamo la NN, possiamo poi semplicemente usare i pesi

*/


#include <chrono>

int MinimaxAgent::utility(GameState state, Board& board) {
    //TODO: implement a decent heuristic

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
    return board.getScore(color);
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
    if (valids.size() == 1) {
        return todo_action;
    }

    /*cout << "Initial: ";
    cout << board.moves.size() << endl;
    */

    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < valids.size(); i++) {
        // Play the move on the copy
        board.executeAction(ActionToString(valids[i]));

        /*cout << "--------------------------UP-----------------------------" << endl;

        cout << "Executing one action: ";
        cout << board.moves.size() << endl;
        */
        
        // Try a simplified call first
        int eval = this->minmax(board.state, board, 1, alpha, beta);
        
        
        if (eval > max_eval) {
            max_eval = eval;
            todo_action = valids[i];
        }
        
        alpha = std::max(alpha, max_eval);

        board.undo(1);

        /*
        cout << "Removing the action just tried: ";
        cout << board.moves.size() << endl;

        cout << "--------------------------DOWN-----------------------------" << endl;
        */
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

        /*

        cout << "--------------------------UP-----------------------------" << endl;

        cout << "Executing one action: ";
        cout << board.moves.size() << endl;

        */

        
        // Recursive call

        int eval = minmax(board.state, board, depth + 1, -beta, -alpha);
        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        board.undo(1);

        /*

        cout << "Removing the action just tried: ";
        cout << board.moves.size() << endl;

        cout << "--------------------------DOWN-----------------------------" << endl;
        */
        
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

    // Initial moves
    if (board.currentTurn <= 4) {
        return board.suggestInitialMove();
    }

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
    
        //cout << "Time taken by function: " << duration.count()/1e6 << " seconds" << endl;
    
    return _cache;
}