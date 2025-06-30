#include "embadded_board.h"
#include <chrono>
#include <iostream>
/*
#include "engine/action.cpp"
#include "engine/board.cpp"
#include "engine/direction.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "graph_board.cpp"
#include "build_from_graph.cpp"
*/



using namespace std;


int main(){
    auto t_start=chrono::high_resolution_clock::now();
    int mbs=0;
    int i=0;
    int totMov=0;
        
    for(int TR=0;TR<200;TR++){
        auto seed=time(0);
        cout<<"Run with seed:"<<seed<<endl;

        actionT ris[256];
        EBoard eb(GameType::Base_MLP);
        int rt;
        try{
        i=0;
        srand(seed+rt);
        
        EBoard eb(GameType::Base_MLP);
        i=0;
        while(1){
            eb.getNextsActions(ris);
            int m=rand()%ris[0]+1;
            //m=1;
                
                
            eb.applyAction(ris[m]);

            //eb.checkConsistency();
            if(eb.getState()>1){
                cout<<eb.getState()<<endl;
                cout<<eb.board_exp.currentTurn<<endl;
                cout<<rt<<": X"<<endl;
                break;
            }
            cout<<"No";

            totMov++;
            i++;
        }
        }catch (exception e){
        cout<<"ERRORE :"<<e.what()<<endl;
        cout<<"More info : i="<<i<<", rt="<<rt<<", seed="<<seed<<endl;
    }
    }

    auto t_end=chrono::high_resolution_clock::now();  
    cout<<"Tempo totale mossa  "<<chrono::duration<double,milli>(t_end-t_start).count()/1000.0/((double)totMov)<<endl;
        cout<<"Mosse eseguite:"<<totMov<<endl;
    
    


}


/*

More info : i=609, rt=0, seed=1745452058
Run with seed:1745452058
ERRORE :std::exception
*/