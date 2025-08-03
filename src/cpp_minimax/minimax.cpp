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

#include <omp.h>

#include <future>
#include <vector>
#include <atomic>
#include <mutex>



std::atomic<bool> time_up_flag(false);

bool MinimaxAgent::is_time_up() const{
    return (std::chrono::high_resolution_clock::now() - start_time )>= timeLimit;
}


std::atomic<long> toale_evaled{0};

double MinimaxAgent::parallel_minimax_first(Board board, int depth_remaining, double alpha, double beta) const {
    return minmax(board,depth_remaining,alpha,beta);
}

actionT MinimaxAgent::initiate_minimax_parallel(Board &board,int depth) {
    
    double best_eval = MIN_EVAL;
    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;

    board.ComputePossibleMoves();
    

    actionT best_move = board.resAction[0];
    time_up_flag.store(false);

    int numAction=board.numAction;
    vector<actionT> nextAct(numAction);
    for(int i=0;i<numAction;i++)
        nextAct[i]=board.resAction[i];

    vector<pair<double,int>> vectSort(numAction);

    for(int i=0;i<numAction;i++){
        board.applayAction(nextAct[i]);
        vectSort[i].first=utility(board);
        board.undoAction();
        vectSort[i].second=i;
    }

    sort(vectSort.begin(),vectSort.end());
    reverse(vectSort.begin(),vectSort.end());

    int NP=omp_get_num_procs();
    vector<double> results(NP);
    for(int j=0;j<numAction;j+=NP){
        int numPar=min(NP,numAction-j);
        
        #pragma omp parallel for
        for (int i = 0; i < numPar; i++) {
            int Q=vectSort[i+j].second;
            actionT move = nextAct[Q];
            Board child(board); // Copy constructor
            child.applayAction(move);
            results[i] = -minmax(child, depth - 1, -beta, -alpha);
        }

        for (int i=0;i<numPar;i++) {

            if (results[i] > best_eval) {
                best_eval = results[i];
                best_move = nextAct[vectSort[i+j].second];;
            }
            alpha = std::max(alpha, best_eval);
        }

    }
    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_eval<<" and depth "<<depth<<endl;
    return best_move;
}


double MinimaxAgent::utility(Board &board) const {
    toale_evaled.fetch_add(1, std::memory_order_relaxed);
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
    clog<<"Evalueted "<<toale_evaled.load()<<" board, found solution "<<todo_action<<" with value "<<max_eval<<" and depth "<<depth<<endl;

    
    return todo_action;
}


double MinimaxAgent::minmax( Board &board, int depth_remaining, double alpha, double beta) const {
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
    toale_evaled.store(0);
    time_up_flag.store(false);
    start_time = std::chrono::high_resolution_clock::now();


    if (board.currentTurn <= 2) {
        clog<<"Suggested";
        return board.suggestInitialMove();
    }

    board.ComputePossibleMoves();
    actionT next_move=board.resAction[0];

    int depth=3;
    while(!is_time_up()){
        actionT ris = initiate_minimax_parallel(board,depth);
        if(!is_time_up()){
            next_move=ris;
            depth++;
        }
    }

    
    return next_move;
}