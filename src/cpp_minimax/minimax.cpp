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



int total_sorted;
int pruned_sorted;
std::atomic<bool> time_up_flag(false);

bool MinimaxAgent::is_time_up() const{
    return (std::chrono::high_resolution_clock::now() - start_time )>= timeLimit;
}

double MinimaxAgent::parallel_minimax_first(Board board, int depth_remaining, double alpha, double beta) {
    return parallel_minmax(board,depth_remaining,alpha,beta);
}

double MinimaxAgent::parallel_minmax(Board &board, int depth_remaining, double alpha, double beta) {
    if (time_up_flag.load() || is_time_up()  ) return 0;

    GameState state = board.getGameState();

    if (state == GameState::DRAW || state == GameState::WHITE_WIN ||
        state == GameState::BLACK_WIN || depth_remaining == 0) {
        return utility( board);
    }

    board.ComputePossibleMoves();

    double max_eval = MIN_EVAL;
    actionT validActions[MAX_ACTIONS_SIZE];
    int numAction=board.numAction;
    for(int i=0;i<numAction;i++){
        validActions[i]=board.resAction[i];
    }

    for (int i = 0; i < numAction; i++) {
        board.applayAction(validActions[i]);
        double eval = -parallel_minmax(board, depth_remaining - 1, -beta, -alpha);
        board.undoAction();

        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, max_eval);

        if (beta <= alpha || is_time_up() || time_up_flag.load()) break;
    }

    return max_eval;
}

actionT MinimaxAgent::initiate_minimax_parallel(Board &board,int depth) {
    board.ComputePossibleMoves();
    
    if (board.numAction == 0) return pass();

    double best_eval = MIN_EVAL;
    actionT best_move = board.resAction[0];


    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;

    std::vector<std::future<std::pair<double, actionT>>> futures;
    
    for (int i = 0; i < board.numAction; i++) {
        Board next_board(board);
        next_board.applayAction(board.resAction[i]);
        futures.push_back(std::async(std::launch::async, [&, i](){
            double eval = -parallel_minmax(next_board, depth - 1, -beta, -alpha);
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

    //cout << "Total evaluated moves: " << toale_evaled << endl;
    return best_move;
}


double MinimaxAgent::utility(Board &board) {
    toale_evaled++;
    //double v1=ev.evalBoardCurrentPlayer(board);
    //double v2=board.getScore();
    
    return board.getScore();
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
    
    if (board.numAction <= 1) {
        return todo_action;
    }
    int saved[MAX_ACTIONS_SIZE];
    for(int i=0;i<board.numAction;i++){
        saved[i]=board.resAction[i];
    }
    int saved_idx=0;
    
    // For every action available, play it and calculate the utility (recursively)
    for (int i = 0; i < board.numAction; i++) {
        Board b1(board);
        b1.applayAction(board.resAction[i]);

        int eval = -minmax(b1.getGameState(), b1, depth-1, -beta, -alpha);

        if (eval > max_eval) {
            max_eval = eval;
            todo_action = board.resAction[i];
            saved_idx=i;
        }
        
        alpha = std::max(alpha, max_eval);
    }

    int f=0;
    for(int i=0;i<board.numAction;i++){
        if(todo_action==saved[i])f++;
    }  
    if(f==0){
        cout<<"Impossible: move not found real"<<endl;

        board.printBoard();
    }
    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<todo_action<<" with value "<<max_eval<<" and depth "<<depth<<endl;
    
    return todo_action;
}


double MinimaxAgent::minmax(GameState state, Board &board, int depth_remaining, double alpha, double beta) {

    if (is_time_up()) {
        return 0; 
    }

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
    
    if(depth_remaining>1){

        vector<pair<int,double>> actPair;
        vector<Board> nextBoards;
        for(int i=0;i<numAction;i++){
            Board nextBoard(board);
            nextBoard.applayAction(board.resAction[i]);
            actPair.push_back(make_pair(utility(nextBoard),i));
            nextBoards.push_back(nextBoard);
        }    

        sort(actPair.begin(),actPair.end());
        total_sorted+=numAction;
        for (int i = numAction-1; i >=0 ; i--) {
            int j=actPair[i].second;

            double eval = -minmax(nextBoards[j].getGameState(), nextBoards[j], depth_remaining - 1, -beta, -alpha);
           
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, max_eval);

            if (beta <= alpha) {
                pruned_sorted+=i;
                break;
            }
        }
    }else{
        actionT validActions[MAX_ACTIONS_SIZE];
        for(int i=0;i<MAX_ACTIONS_SIZE;i++){
            validActions[i]=board.resAction[i];
        }

        for (int i = 0; i < numAction; i++) {

            board.applayAction(validActions[i]);
            double eval = -minmax(board.getGameState(), board, depth_remaining - 1, -beta, -alpha);
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
    toale_evaled = 0;
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

    int depth=3;
    while(!is_time_up()){
        actionT ris = initiate_minimax_fixed(board,depth);
        if(!is_time_up()){
            next_move=ris;
            depth++;
        }
    }

    cout<<"Improvement "<<pruned_sorted<<" over "<<total_sorted<<endl;
    
    return next_move;
}