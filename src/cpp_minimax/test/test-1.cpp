#include "../engine/board.h"
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
        
    auto seed=time(0);
    for(int TR=0;TR<20;TR++){
        cout<<"Run with seed:"<<seed+TR<<endl;
        srand(seed+TR);

        Board eb(GameType::Base_MLP);
        int rt;
        try{
        i=0;
        
        while(1){
            eb.ComputePossibleMoves();
            if(eb.numAction==0){

            }
            int m=rand()%eb.numAction;
            //m=1;
                
                
            eb.applayAction(eb.resAction[m]);

            //eb.checkConsistency();
            if(eb.getGameState()>1){
                cout<<eb.getGameState()<<endl;
                cout<<eb.currentTurn<<endl;
                cout<<rt<<": X"<<endl;
                break;
            }

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