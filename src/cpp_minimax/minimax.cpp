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

/*double MinimaxAgent::parallel_minimax_first(Board board, int depth_remaining, double alpha, double beta) const {
    return minmax(board,depth_remaining,alpha,beta);
}*/

actionT MinimaxAgent::initiate_minimax_parallel(Board &board,int depth) {
    GameState state = board.getGameState();
    if (state == GameState::DRAW    ||
        state == GameState::WHITE_WIN||
        state == GameState::BLACK_WIN) {
        return pass();
    }
    double best_value = MIN_EVAL;
    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;

    board.ComputePossibleMoves();
    

    actionT best_move = board.resAction[0];
    time_up_flag.store(false);

    int numAction=board.numAction;
    vector<actionT> nextAct(numAction);
    for(int i=0;i<numAction;i++)
        nextAct[i]=board.resAction[i];

    vector<pair<double,actionT>> vectSort(numAction);

    for(int i=0;i<numAction;i++){
        board.applayAction(nextAct[i]);
        vectSort[i]=make_pair(utility(board),nextAct[i]);
        board.undoAction();
    }

    sort(vectSort.begin(),vectSort.end());
    //reverse(vectSort.begin(),vectSort.end());

    double eval=0;
    for(int j=0;j<numAction;j++){
        
        actionT move = vectSort[j].second;
        board.applayAction(move);
        eval= -minmax2(board, depth - 1, -beta, -alpha);
        board.undoAction();

        if (eval > best_value) {
            best_value = eval;
            best_move =move;;
        }
        alpha = std::max(alpha, eval);
        if (alpha >= beta) {
            break;
        }   
    }
    clog<<"Evaluated "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_value<<" and depth "<<depth<<endl;
    return best_move;
}


double MinimaxAgent::utility(Board &board) const {
    toale_evaled.fetch_add(1, std::memory_order_relaxed);
    double v1=ev.evalBoardCurrentPlayer(board);
    return v1;
}




double MinimaxAgent::minmax2( Board &board, int depth_remaining, double alpha, double beta) const {
    if(is_time_up()){return MAX_EVAL;}
        
    //======================
    //      BASE CASE
    //======================
    GameState state = board.getGameState();
    if (state == GameState::DRAW    ||
        state == GameState::WHITE_WIN||
        state == GameState::BLACK_WIN||
        depth_remaining == 0) {
        return utility(board);
    }


    //========================================================
    //     non base case: define best and next acts
    //========================================================

    board.ComputePossibleMoves();

    double best_value=MIN_EVAL;
    int n=board.numAction;    
    vector<actionT> nextActs(n);

    for(int i=0;i<n;i++) nextActs[i]=board.resAction[i];

    //======================================================
    //  Trivial case: not sorted, evaluate directly
    //======================================================
    if(depth_remaining==1){            
        int NP=omp_get_num_procs();

        for (int i = 0; i < n; i+=NP) {


            int nl=min(NP,n-i);
            vector<double> ut(nl);

            #pragma opm parallel for
            for(int j=0;j<nl;j++){
                Board b1(board,nextActs[i+j]);
                ut[j] = -utility(b1);
            }

            for(int j=0;j<nl;j++){
                best_value = std::max(best_value, ut[j]);
                alpha = std::max(alpha, ut[j]);
                if (alpha>=beta) {
                    return best_value;  //THIS LINE
                }
            }
            
            
        } 
        return best_value;
    }

    //========================================================
    //      Sort the variables based on utils
    //========================================================
    vector<pair<double,actionT>> sorted_pair;
    sorted_pair.reserve(n);
    for(int i=0;i<n;i++){
        board.applayAction(nextActs[i]);
        sorted_pair.push_back(make_pair(utility(board),nextActs[i]));
        board.undoAction();
    }
    sort(sorted_pair.begin(),sorted_pair.end());
    //reverse(sorted_pair.begin(),sorted_pair.end());

    
    
    //========================================================
    //      Non parallel case: execute all
    //========================================================
    if(depth_remaining>0){
        for(int i=0;i<n;i++){
            board.applayAction(sorted_pair[i].second);
            double score=-minmax2(board,depth_remaining-1,-beta,-alpha);
            board.undoAction();
            best_value=max(best_value,score);
            alpha=max(alpha,score);

            if(alpha>=beta){
                return beta;
            }
        }
        return best_value;
    }

    //clog<<"NO"<<endl;throw 2;
    //========================================================
    //      Parallel case: execute in parallel for level 2
    //========================================================

    int NP=omp_get_num_procs();
    vector<double> res(NP);
    for(int k=0;k<n;k+=NP){
        int up=min(n-k,NP);
        //#pragma omp parallel for 
        for (int i = 0; i <up ; i++) {
            actionT move=sorted_pair[k+i].second;
            Board b1(board);
            b1.applayAction(move);
            res[i] = -minmax2( b1, depth_remaining - 1, -beta, -alpha);
        }

        for (int i = 0; i <up;i++) {
            best_value = std::max(best_value, res[i]);
            alpha = std::max(alpha, res[i]);
            if(alpha>=beta){
                return best_value;
            }
        }
    }
    return best_value;

}


pair<actionT,double> MinimaxAgent::minimaxComplete(Board board,int d){
    pair<actionT,double> ris=make_pair(0,MIN_EVAL);
    if(d==1){
        board.ComputePossibleMoves();
        for(int i=0;i<board.numAction;i++){
            Board b1(board);
            b1.applayAction(board.resAction[i]);
            double currV=-utility(b1);
            if(currV>ris.second){
                ris=make_pair(board.resAction[i],currV);
            }
        }
    }else{
        board.ComputePossibleMoves();
        for(int i=0;i<board.numAction;i++){
            Board b1(board);
            b1.applayAction(board.resAction[i]);
            double val=-minimaxComplete(b1,d-1).second;
            if(val>ris.second){
                ris=make_pair(board.resAction[i],val);
            }
        }
    }

    return ris;
}

actionT MinimaxAgent::initiate_minimax_learning(Board &board,int depth, double learning_fact) {

    GameState state = board.getGameState();
    if (state == GameState::DRAW    ||
        state == GameState::WHITE_WIN||
        state == GameState::BLACK_WIN) {
        return pass();
    }
    double best_value = MIN_EVAL;
    double alpha = MIN_EVAL;
    double beta = MAX_EVAL;

    board.ComputePossibleMoves();
    

    actionT best_move = board.resAction[0];
    time_up_flag.store(false);

    int numAction=board.numAction;
    vector<actionT> nextAct(numAction);
    for(int i=0;i<numAction;i++)
        nextAct[i]=board.resAction[i];

    vector<pair<double,actionT>> vectSort(numAction);
    
    for(int i=0;i<numAction;i++){
        board.applayAction(nextAct[i]);
        vectSort[i]=make_pair(utility(board),nextAct[i]);
        board.undoAction();
    }

    sort(vectSort.begin(),vectSort.end());

    double eval=0;
    vector<double> ris(numAction);
    for(int j=0;j<numAction;j++){
        actionT move = vectSort[j].second;
        board.applayAction(move);
        ris[j]= -minmax2(board, depth - 1, -beta, -alpha);
        board.undoAction();    
    }

    for(int j=0;j<numAction;j++){
        ris[j]=pow(learning_fact,ris[j]/1000);
    }
    double s=0;
    for(int j=0;j<numAction;j++){
        s+=ris[j];
    }
    for(int i=0;i<numAction;i++){
        ris[i]/=s;
    }

    double val=((double)(rand()%100000))/100000.0;
    double cs=0;
    int i=0;
    while(cs<val){
        cs+=ris[i];
        i++;
    }
    if(i==0)i++;
    if(i>numAction){
        cout<<"NO"<<endl;
        //throw "Not valid";
    }
    return nextAct[i-1];
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

    int depth=2;
    
    while(!is_time_up()){
        actionT ris =  initiate_minimax_parallel(board,depth); //initiate_minimax_parallel(board,depth);
        if(!is_time_up()){
            next_move=ris;
            depth++;
        }
    }

    
    return next_move;
}