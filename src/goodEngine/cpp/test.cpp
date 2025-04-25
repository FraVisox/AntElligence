#include "embadded_board.h"
#include <chrono>
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
    for(int TR=0;TR<1;TR++){
        auto seed=time(0);
        cout<<"Run with seed:"<<seed<<endl;

        srand(seed);
        auto t_start=chrono::high_resolution_clock::now();
        int mbs=0;
        int i=0;
        int totMov=0;
        actionT ris[256];
        EBoard eb(GameType::Base);
        int rt;
        try{
        i=0;
        for(rt=0;rt<10;rt++){
            EBoard eb(GameType::Base);
            i=0;
            while(1){
                
                eb.getNextsActions(ris);
                if(i==-1){
                    auto t1=chrono::high_resolution_clock::now();
                    for(int q=0;q<1000;q++){
                        eb.getNextsActions(ris);
                    }
                    auto t2=chrono::high_resolution_clock::now();
                    cout<<"Duration of get actions: "<<chrono::duration<double,milli>(t2-t1).count()<<endl;
                }
                int m=rand()%ris[0]+1;
                
                

                eb.applyAction(ris[m]);

                eb.checkConsistency();
                if(eb.getState()!=1){
                    break;
                }


                totMov++;
                i++;
            }
         
        }
        auto t_end=chrono::high_resolution_clock::now();  
        cout<<"Tempo totale mossa  "<<chrono::duration<double,milli>(t_end-t_start).count()/1000.0/((double)totMov)<<endl;
            
        }catch (exception e){
            cout<<"ERRORE :"<<e.what()<<endl;
            cout<<"More info : i="<<i<<", rt="<<rt<<", seed="<<seed<<endl;
        }
    }



}


/*

More info : i=609, rt=0, seed=1745452058
Run with seed:1745452058
ERRORE :std::exception
*/