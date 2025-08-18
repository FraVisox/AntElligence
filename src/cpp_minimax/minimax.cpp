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

std::atomic<long> toale_evaled{0};
int SETUP_depth_pruning;

bool MinimaxAgent::is_time_up() const {
    return (std::chrono::high_resolution_clock::now() - start_time) >= timeLimit;
}



double MinimaxAgent::utility(Board &board) const {
    toale_evaled.fetch_add(1, std::memory_order_relaxed);
    return ev.evalBoardCurrentPlayer(board);
}

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

    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_value<<" and depth "<<depth<<endl;
    return best_move;
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


    //===============================================
    //      Trivial case - non parallel
    //================================================
    if(depth_remaining==1 ){        
        double score;   
        for (int i = 0; i < n; i++) {
            
            board.applayAction(nextActs[i]);
            score = -utility(board);
            board.undoAction();
            best_value = std::max(best_value, score);
            alpha = std::max(alpha, score);

            if (alpha>=beta ){
                return best_value;  
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
        sorted_pair.emplace_back(utility(board),nextActs[i]);
        board.undoAction();
    }
    
    sort(sorted_pair.begin(),sorted_pair.end());
    //reverse(sorted_pair.begin(),sorted_pair.end());

    

    //=======================================================
    //      parallel starting: work for latest depth (e.g. 3)
    //=======================================================
    if(depth_remaining==SETUP_depth_pruning){
        std::atomic<bool> exitAll{false};
        std::atomic<double> alphaAtomic{alpha};
        std::vector<double> resUtil(n, MIN_EVAL);
;  

        #pragma omp parallel for schedule(dynamic,1) 
        for(int i=0;i<n;i++){
            if (exitAll.load(std::memory_order_acquire)) continue;

            
            Board b1(board,sorted_pair[i].second);    
            b1.ComputePossibleMoves();
                
                
                // Sort variables
            const int innAct=b1.numAction;
            vector<actionT> futureActs(b1.resAction,b1.resAction+innAct);


            vector<pair<double,actionT>> sorted_pairInn;
            sorted_pairInn.reserve(innAct);
            for(int q=0;q<innAct;q++){
                b1.applayAction(futureActs[q]);
                sorted_pairInn.emplace_back(utility(b1),futureActs[q]);
                b1.undoAction();
            }
            sort(sorted_pairInn.begin(),sorted_pairInn.end());
                
            double localAlpha = -beta;                 // note: negated window for child (negamax)
            double localBeta  = -alphaAtomic.load(std::memory_order_acquire);
            double best_local = MIN_EVAL;

            // First level search
            for(int q=0;q<innAct;q++){
                if (exitAll.load(std::memory_order_acquire) || is_time_up()) break;

                actionT move=sorted_pairInn[q].second;

                b1.applayAction(move);
                const double score=-minmax2(b1,depth_remaining-2,-localBeta,-localAlpha);
                b1.undoAction();

                if (score > best_local) best_local = score;
                if (best_local > localAlpha) localAlpha = best_local;
                if (localAlpha >= localBeta) break;
                
                const double snapA = alphaAtomic.load(std::memory_order_acquire);
                if (-snapA < localBeta) localBeta = -snapA;

                
            }
            
            resUtil[i] = -best_local; // back to parent perspective
            
            double curA = alphaAtomic.load(std::memory_order_relaxed);
            while (resUtil[i] > curA && !alphaAtomic.compare_exchange_weak(curA, resUtil[i],
                                                                           std::memory_order_release,
                                                                           std::memory_order_relaxed)) {
            }
            if (alphaAtomic.load(std::memory_order_acquire) >= beta) {
                exitAll.store(true, std::memory_order_release);
            }
                
        }
        for (int i = 0; i < n; ++i) {
            if (resUtil[i] > best_value) best_value = resUtil[i];
            if (best_value > alpha) alpha = best_value;
            if (alpha >= beta) break;
        }
        return best_value;
    }


    //========================================================
    //      Non parallel case: execute all
    //========================================================
    
    
    for(int i=0;i<n;i++){
        board.applayAction(sorted_pair[i].second);
        double score=-minmax2(board,depth_remaining-1,-beta,-alpha);
        board.undoAction();
        best_value=max(best_value,score);
        alpha=max(alpha,score);

        if(alpha>=beta){
            return best_value;
        }
    }
    return best_value;
}


pair<actionT,double> MinimaxAgent::minimaxComplete(Board board,int d){
    pair<actionT,double> ris=make_pair(0,MIN_EVAL);
    
    if(d==0){
        return make_pair(0,utility(board));
    }
    board.ComputePossibleMoves();
    for(int i=0;i<board.numAction;i++){
        Board b1(board,board.resAction[i]);
        //b1.applayAction(board.resAction[i]);
        double val=-minimaxComplete(b1,d-1).second;
        if(val>ris.second){
            ris=make_pair(board.resAction[i],val);
        }
    }

    return ris;
}



actionT MinimaxAgent::calculate_best_move(Board &board) {
    toale_evaled.store(0);
    start_time = std::chrono::high_resolution_clock::now();

    board.ComputePossibleMoves();
    actionT next_move=board.resAction[0];


    int depth=3;
  
  //  auto r1=minimaxComplete(board,1);cout<<"d =1 has action "<< r1.first<<" with va "<<r1.second<<" evalueted: "<<toale_evaled<<endl;toale_evaled=0;    
    //auto r2=minimaxComplete(board,2);cout<<"d =2 has action "<< r2.first<<" with va "<<r2.second<<" evalueted: "<<toale_evaled<<endl;toale_evaled=0;
    //auto r3=minimaxComplete(board,3);cout<<"d =3 has action "<< r3.first<<" with va "<<r3.second<<" evalueted: "<<toale_evaled<<endl;toale_evaled=0;    
    //auto r4=minimaxComplete(board,4);cout<<"d =4 has action "<< r4.first<<" with va "<<r4.second<<endl;
    
    //omp_set_num_threads(10); // limit to 4 threads

    while (!is_time_up() && depth < 15) {
        toale_evaled.store(0, std::memory_order_relaxed);
        // Keep your split depth; you can tune it per branching factor
        SETUP_depth_pruning = depth-1;

        const actionT candidate = initiate_minimax_parallel(board, depth);
        if (is_time_up()) break;

        next_move = candidate;
        ++depth;
    }
     
    return next_move;
}
