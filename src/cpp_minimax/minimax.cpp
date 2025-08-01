#include "minimax.h"
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <chrono>
#include <bitset>


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
        //clog<<"Start while "<<endl;
        actionT move_this_depth = best_move;
        int max_eval = MIN_EVAL;
        int alpha = MIN_EVAL;
        int beta = MAX_EVAL;

        //cout << "Current depth: " << current_depth << endl;

        // STEP 1: Move ordering – shallow eval at depth 1
        //std::vector<std::pair<int, actionT>> scored_actions(board.numAction);
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
        return 0;  // Return a neutral evaluation when time runs out
    }

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
        //auto h=board.simple_hash();
        //Board sb(board);
        //clog<<"Applay action "<<validActions[i]<<endl;
        board.applayAction(validActions[i]);
        

        int eval = -minmax(board.getGameState(), board, depth_remaining - 1, -beta, -alpha);
        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        // Alpha-beta pruning
        if (beta <= alpha) {
            board.undoAction();
            break;
        }
        //clog<<"Undo action "<<validActions[i]<<endl;
        board.undoAction();
        /*auto z=board.simple_hash();
        if(h!=z){
            cout<<"Hash is not the same, undo failed"<<endl;

            sb.printBoard();

            cout<<"\n\n Applied action "<<validActions[i]<<"\n\n"<<endl;
            board.printBoard();

            throw "NOT";
        }*/
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

    /*if (DISABLE_CACHE || _cache == pass() || board.currentTurn != cached_turn) {
        color = board.currentColor();
        cached_turn = board.currentTurn;
     */   _cache = initiate_minimax_iterative(board);
    //}


    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();

    // Get duration. Substart timepoints to 
    // get duration. To cast it to proper unit
    // use duration cast method
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    //cout << "Time taken by function: " << duration.count()/1e6 << " seconds with " <<calledBoard << " evaluation" << endl;
    
    return _cache;
}