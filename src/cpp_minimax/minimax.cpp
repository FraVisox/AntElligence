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


int SETUP_depth_pruning;

std::atomic<long> toale_evaled{0};

unordered_map<size_t,double> HMP[10];
double MinimaxAgent::utility(Board &board) const {

    #pragma omp critical(MOR)
    toale_evaled.fetch_add(1, std::memory_order_relaxed);
    size_t H=board.getHash();

    std::unordered_map<size_t, double, std::hash<unsigned long>, std::equal_to<size_t>, std::allocator<std::pair<const size_t, double>>>::iterator c;double r;

    #pragma omp critical(HMP)
    c=HMP[board.currentTurn%10].find(H); 

    if(c!=HMP[board.currentTurn%10].end()){

        return c->second;
    }
    double v1=ev.evalBoardCurrentPlayer(board);
    auto P=make_pair(H,v1);
    #pragma omp critical(HMP)
    HMP[board.currentTurn%10].insert(P);
    return v1;
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

    clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_value<<" and depth "<<depth<<endl;
    return best_move;
}






actionT MinimaxAgent::initiate_minimax_parallel2(Board &board,int depth) {


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
    vector<double> resUtil(numAction);

    #pragma omp parallel for schedule(dynamic,1) 
    for(int i=0;i<numAction;i++){

        double alphaInn=-beta;
        double betaInn=-alpha;
        bool exit=false;
        double best_valueInn = MIN_EVAL;

        // ===================================
        // EXPLICIT FIRST LEVEL OF COMPUTATION
        // ===================================
        Board b1(board,vectSort[i].second);    
        b1.ComputePossibleMoves();


        // Sort variables
        int innAct=b1.numAction;
        vector<actionT> futureActs(innAct);
        for(int q=0;q<innAct;q++)
            futureActs[q]=b1.resAction[q];
        
        vector<pair<double,actionT>> sorted_pair;
        sorted_pair.reserve(innAct);
        for(int q=0;q<innAct;q++){
            b1.applayAction(futureActs[q]);
            sorted_pair.emplace_back(utility(b1),futureActs[q]);
            b1.undoAction();
        }
        sort(sorted_pair.begin(),sorted_pair.end());
        

        double tempAlpha;
        // First level search
        for(int q=0;q<innAct;q++){

            
            #pragma omp atomic read 
            tempAlpha=alpha;
            
            betaInn=min(-tempAlpha,betaInn);

            b1.applayAction(sorted_pair[q].second);
            double score=-minmax2(b1,depth-2,-betaInn,-alphaInn);
            b1.undoAction();
            best_valueInn=max(best_valueInn,score);
            alphaInn=max(alphaInn,score);

            if(alphaInn>=betaInn){
                resUtil[i]=-betaInn;

                break;
            }
        }

        resUtil[i]=-best_valueInn;

        #pragma omp critical(alpha_update)
        {
            if (-best_valueInn > alpha) alpha = -best_valueInn;
        }
    }
    
    for(int i=0;i<numAction;i++){
        if(resUtil[i]>best_value){
            best_value=resUtil[i];
            best_move=vectSort[i].second;
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
        
    
        vector<double> resUtil(n);
        bool exitAll=false;

        #pragma omp parallel for schedule(dynamic,1) 
        for(int i=0;i<n;i++){

            resUtil[i]=MIN_EVAL;
            double alphaInn=-beta;
            double betaInn=-alpha;
            double best_valueInn = MIN_EVAL;
            bool localExit;
            #pragma omp critical(exit)
            localExit=exitAll;
                    
            if(!localExit){
                    

                // ===================================
                // EXPLICIT FIRST LEVEL OF COMPUTATION
                // ===================================
                
                Board b1(board,sorted_pair[i].second);    
                b1.ComputePossibleMoves();
                
                
                // Sort variables
                int innAct=b1.numAction;
                vector<actionT> futureActs(b1.resAction,b1.resAction+innAct);


                vector<pair<double,actionT>> sorted_pairInn;
                sorted_pairInn.reserve(innAct);
                for(int q=0;q<innAct;q++){
                    b1.applayAction(futureActs[q]);
                    sorted_pairInn.emplace_back(utility(b1),futureActs[q]);
                    b1.undoAction();
                }
                sort(sorted_pairInn.begin(),sorted_pairInn.end());
                

                double tempAlpha;
                // First level search
                for(int q=0;q<innAct;q++){
                    
                    
                    #pragma omp critical(exit)
                    localExit=exitAll;
                    if(localExit)
                        break;

                    #pragma omp atomic read 
                    tempAlpha=alpha;
                    
                    betaInn=min(-tempAlpha,betaInn);

                    b1.applayAction(sorted_pairInn[q].second);
                    double score=-minmax2(b1,depth_remaining-2,-betaInn,-alphaInn);
                    b1.undoAction();
                    best_valueInn=max(best_valueInn,score);
                    alphaInn=max(alphaInn,score);

                    if(alphaInn>=betaInn){
                        break;
                    }
                }
                resUtil[i]=-best_valueInn;

                

                #pragma omp critical(alpha_update)
                {
                    if(best_value<resUtil[i])best_value=resUtil[i];
                    if (alpha<resUtil[i]) alpha =resUtil[i];

                    if(alpha>=beta){
                        exitAll=true;
                    }
                }
            }
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
    if(d==1){
        board.ComputePossibleMoves();
        for(int i=0;i<board.numAction;i++){
            Board b1(board,board.resAction[i]);
            b1.applayAction(board.resAction[i]);
            double currV=-utility(b1);
            if(currV>ris.second){
                ris=make_pair(board.resAction[i],currV);
            }
        }
    }else{
        board.ComputePossibleMoves();
        for(int i=0;i<board.numAction;i++){
            Board b1(board,board.resAction[i]);
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


    //clog<<"Evalueted "<<toale_evaled<<" board, found solution "<<best_move<<" with value "<<best_value<<" and depth "<<depth<<endl;
    return best_move;
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
    
    //auto r2=minimaxComplete(board,2);cout<<"d =2 has action "<< r2.first<<" with va "<<r2.second<<endl;
    //auto r3=minimaxComplete(board,3);cout<<"d =3 has action "<< r3.first<<" with va "<<r3.second<<endl;    
    //auto r4=minimaxComplete(board,4);cout<<"d =4 has action "<< r4.first<<" with va "<<r4.second<<endl;
    
    //omp_set_num_threads(3); // limit to 4 threads
    while(!is_time_up() && depth <15){
        SETUP_depth_pruning=3;
        actionT ris =initiate_minimax_parallel(board,depth);
        //SETUP_depth_pruning=-1;
        //ris =  initiate_minimax_parallel(board,depth);
         //auto r4=minimaxComplete(board,depth);cout<<"d ="<<depth<<" has action "<< r4.first<<" with va "<<r4.second<<endl;
    
        if(!is_time_up()){
            next_move=ris;
            depth++;
        }
    }

    HMP[board.currentTurn%10].clear();
    HMP[(board.currentTurn+1)%10].clear();
    
    return next_move;
}