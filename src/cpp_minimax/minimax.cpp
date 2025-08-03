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
#include "TT.h"


#include <future>
#include <vector>
#include <atomic>
#include <mutex>



int total_sorted;
int pruned_sorted;
std::atomic<bool> time_up_flag(false);

bool MinimaxAgent::is_time_up() const{
    return (std::chrono::high_resolution_clock::now() - start_time )>= timeLimit;
}

double MinimaxAgent::parallel_minimax_first(Board board, int depth_remaining, double alpha, double beta) {
    return minmax(board,depth_remaining,alpha,beta);
}

actionT MinimaxAgent::initiate_minimax_parallel(Board &board,int depth) {
    
    double best_eval = MIN_EVAL;
    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;

    board.ComputePossibleMoves();
    

    actionT best_move = board.resAction[0];
    time_up_flag.store(false);



    std::vector<std::future<std::pair<double, actionT>>> futures;
    
    for (int i = 0; i < board.numAction; i++) {
        actionT move = board.resAction[i];
        Board child(board);
        child.applayAction(move);
        futures.push_back(std::async(std::launch::async,
        [child, move, depth, alpha, beta, this](){
            // If you need to refer to members (like time_up_flag) use 'this' and thus capture it.
            double eval = -parallel_minimax_first(child, depth - 1, -beta, -alpha);
            return std::make_pair(eval, move);
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

    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_eval<<" and depth "<<depth<<endl;
    return best_move;
}


double MinimaxAgent::utility(Board &board) {
    toale_evaled++;
    double v1=ev.evalBoardCurrentPlayer(board);
    return v1;
    //double v2=board.getScore();
    
    //return board.getScore();
}


//FIXED DEPTH:
actionT MinimaxAgent::initiate_minimax_fixed(Board &board,int depth) {
    double max_eval = MIN_EVAL;
    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;
    actionT todo_action = pass();

    // Get all valid moves
    board.ComputePossibleMoves();

    todo_action = board.resAction[0];    
    time_up_flag.store(false);

    
    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < board.numAction; i++) {
        Board b1(board);
        b1.applayAction(board.resAction[i]);

        int eval = -minmax(b1, depth-1, -beta, -alpha);

        if (eval > max_eval) {
            max_eval = eval;
            todo_action = board.resAction[i];
        }
        
        alpha = std::max(alpha, max_eval);
    }
    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<todo_action<<" with value "<<max_eval<<" and depth "<<depth<<endl;

    
    return todo_action;
}


double MinimaxAgent::minmax( Board &board, int depth_remaining, double alpha, double beta) {
    if (time_up_flag.load() || is_time_up()  ) return 0;

    GameState state=board.getGameState();

    // Check if we've reached a terminal state or maximum depth
    if (state == GameState::DRAW || 
        state == GameState::WHITE_WIN || 
        state == GameState::BLACK_WIN || 
        depth_remaining == 0) {

        // Return an evaluation of the board
        return utility(board);
    }
    
    

    board.ComputePossibleMoves();
    

    double max_eval = MIN_EVAL;

    int numAction=board.numAction;
    
    actionT validActions[MAX_ACTIONS_SIZE];
    for(int i=0;i<MAX_ACTIONS_SIZE;i++){
        validActions[i]=board.resAction[i];
    }
    if(depth_remaining>1){

        vector<pair<int,double>> actPair;
        //vector<Board> nextBoards;

        for(int i=0;i<numAction;i++){

            board.applayAction(validActions[i]);
            actPair.push_back(make_pair(utility(board),i));
            board.undoAction();
        }    

        sort(actPair.begin(),actPair.end());
        total_sorted+=numAction;

        for (int i = numAction-1; i >=0 ; i--) {
            int j=actPair[i].second;
            board.applayAction(validActions[j]);
            double eval = -minmax( board, depth_remaining - 1, -beta, -alpha);
            board.undoAction();
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);

            if (beta <= alpha) {
                break;
            }
        }
    }else{
        

        for (int i = 0; i < numAction; i++) {

            board.applayAction(validActions[i]);
            double eval = -utility(board);
            board.undoAction();
            
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);

            if (beta <= alpha) {
                break;
            }
        }

    }
    return max_eval;
}




actionT MinimaxAgent::calculate_best_move(Board &board) {
    toale_evaled=toale_evaled2 = 0;
    time_up_flag.store(false);
    total_sorted=0;
    pruned_sorted=0;
    start_time = std::chrono::high_resolution_clock::now();


    if (board.currentTurn <= 2) {
        clog<<"Suggested";
        return board.suggestInitialMove();
    }

    board.ComputePossibleMoves();
    actionT next_move=board.resAction[0];

    int depth=1;
    while(!is_time_up()){
        actionT ris = initiate_minimax_parallel(board,depth);
        if(!is_time_up()){
            next_move=ris;
            depth++;
        }
    }

    clog<<"Improvement "<<pruned_sorted<<" over "<<total_sorted<<endl;
    
    return next_move;
}