#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>
#include <bitset>
#include <future>
#include <vector>
#include <atomic>
#include <mutex>


#include <future>
#include <vector>
#include <atomic>
#include <mutex>

std::atomic<bool> time_up_flag(false);

int MinimaxAgent::parallel_minmax(Board &board, int depth_remaining, int alpha, int beta) {
    if (is_time_up() || time_up_flag.load()) return 0;

    GameState state = board.getGameState();

    if (state == GameState::DRAW || state == GameState::WHITE_WIN ||
        state == GameState::BLACK_WIN || depth_remaining == 0) {
        return utility(state, board);
    }

    board.ComputePossibleMoves();

    int max_eval = MIN_EVAL;
    actionT validActions[MAX_ACTIONS_SIZE];
    int numAction=board.numAction;
    for(int i=0;i<numAction;i++){
        validActions[i]=board.resAction[i];
    }

    for (int i = 0; i < numAction; i++) {
        board.applayAction(validActions[i]);
        int eval = -parallel_minmax(board, depth_remaining - 1, -beta, -alpha);
        board.undoAction();

        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        if (beta <= alpha || is_time_up() || time_up_flag.load()) break;
    }

    return max_eval;
}

actionT MinimaxAgent::initiate_minimax_iterative_parallel(Board &board) {
    toale_evaled = 0;
    time_up_flag.store(false);
    //clog << "Start parallel minimax" << endl;
    board.ComputePossibleMoves();
    //clog << "Computed " << board.numAction << endl;

    if (board.numAction == 0) return pass();

    int best_eval = MIN_EVAL;
    actionT best_move = board.resAction[0];

    int current_depth = 1;

    while (!is_time_up()) {
        int alpha = MIN_EVAL;
        int beta = MAX_EVAL;

        std::vector<std::future<std::pair<int, actionT>>> futures;
        vector<Board> nextBoards(board.numAction,board);

        for (int i = 0; i < board.numAction; i++) {
            
            nextBoards[i].applayAction(board.resAction[i]);
        }
        for (int i = 0; i < board.numAction; i++) {
            futures.push_back(std::async(std::launch::async, [&, i](){
                int eval = -parallel_minmax(nextBoards[i], current_depth - 1, -beta, -alpha);
                return std::make_pair(eval, board.resAction[i]);
            }));
        }

        for (auto &f : futures) {
            if (is_time_up()) {
                time_up_flag.store(true);
                break;
            }

            auto [eval, move] = f.get();
            if (eval > best_eval) {
                best_eval = eval;
                best_move = move;
            }
            alpha = std::max(alpha, best_eval);
        }

        //clog << "Current best: " << best_move << " at depth " << current_depth << endl;
        current_depth++;
    }

    //cout << "Total evaluated moves: " << toale_evaled << endl;
    return best_move;
}


int MinimaxAgent::utility(GameState state, Board &board) {
    toale_evaled++;
    return board.getScore(color);
}

//ITERATIVE DEEPENING:
actionT MinimaxAgent::initiate_minimax_iterative(Board &board) {
    toale_evaled=0;
    clog<<"Stard minimax"<<endl;
    board.ComputePossibleMoves();
    clog<<"Computed "<<board.numAction<<endl;
    if (board.numAction == 0) return pass();

    int best_eval = MIN_EVAL;
    actionT best_move = board.resAction[0];

    int current_depth = 1;

    while (!is_time_up()) {
        actionT move_this_depth = best_move;
        int max_eval = MIN_EVAL;
        int alpha = MIN_EVAL;
        int beta = MAX_EVAL;

        
        std::vector<Board> nextBoards(board.numAction, board);

        for (int i=0;i<board.numAction;i++) {
            nextBoards[i].applayAction(board.resAction[i]);
        }

        pair<int,int> vals[board.numAction];
        for(int i=0;i < board.numAction;i++){
            
            vals[i]=make_pair(i,utility(nextBoards[i].getGameState(),nextBoards[i]));
        }

        actionT risAct[board.numAction];

        // Sort actions descending by score (most promising first)
        std::sort(vals,vals+board.numAction,
                  [](const auto& a, const auto& b) { return a.second > b.second; });

        // Replace actions list with sorted version
        

        // STEP 2: Do actual search at current depth
        for (int qi=0;qi<board.numAction;qi++) {
            int i=vals[qi].first;
            if (is_time_up()) break;

            int eval = -minmax(nextBoards[i].getGameState(), nextBoards[i], current_depth, -beta, -alpha);
            if (eval > max_eval) {
                max_eval = eval;
                move_this_depth = board.resAction[i];
            }
            alpha = std::max(alpha, max_eval);
        }

        // If time was not up, accept this result
        if (!is_time_up() || (max_eval>best_eval)) {
                best_eval = max_eval;
                best_move = move_this_depth;
            }
        clog<<"Current best: "<<best_move<< "  at depthe"<<current_depth <<endl;;
        current_depth++; // Increase search depth for next round
    }
    cout<<"Total evaled moves: "<<toale_evaled;
    return best_move;
}

//FIXED DEPTH:
actionT MinimaxAgent::initiate_minimax_fixed(Board board) {
    int max_eval = MIN_EVAL;
    int alpha = MIN_EVAL;
    int beta = MAX_EVAL;
    actionT todo_action = pass();

    // Get all valid moves
    board.ComputePossibleMoves();

    if (board.numAction != 0) {
        todo_action = board.resAction[0];
    }
    if (board.numAction <= 1) {
        return todo_action;
    }

    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < board.numAction; i++) {
        // Play the move on the copy
        Board b1(board);
        b1.applayAction(board.resAction[i]);

        int eval = -minmax(b1.getGameState(), b1, depthLimit-1, -beta, -alpha);

        if (eval > max_eval) {
            max_eval = eval;
            todo_action = board.resAction[i];
        }
        
        alpha = std::max(alpha, max_eval);

        if (beta <= alpha) {
            break;
        }
    }
    
    return todo_action;
}

int MinimaxAgent::minmax(GameState state, Board &board, int depth_remaining, int alpha, int beta) {

    if (is_time_up()) {
        return 0;  }

    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth_remaining == 0) {

        // Return an evaluation of the board
        return utility(state, board);
    }
    
    board.ComputePossibleMoves();
    
    int max_eval = MIN_EVAL;
    actionT validActions[MAX_ACTIONS_SIZE];
    int numAction=board.numAction;
    for(int i=0;i<numAction;i++){
        validActions[i]=board.resAction[i];
    }
    for (int i = 0; i < numAction; i++) {
        board.applayAction(validActions[i]);
        int eval = -minmax(board.getGameState(), board, depth_remaining - 1, -beta, -alpha);
        board.undoAction();
        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        if (beta <= alpha) {
            break;
        }
        board.undoAction();
    }
    return max_eval;
}

actionT MinimaxAgent::calculate_best_move(Board &board) {

    start_time = std::chrono::high_resolution_clock::now();

    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();
    calledBoard=0;

    // Initial moves
    if (board.currentTurn <= 2) {
        clog<<"Suggested";
        return board.suggestInitialMove();
    }

       _cache = initiate_minimax_iterative_parallel(board);
    

    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    
    return _cache;
}