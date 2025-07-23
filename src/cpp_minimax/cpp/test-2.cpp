#include "embadded_board.h"
#include "interface.h"
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
    try{
    
    for(int TR=0;TR<1;TR++){
        auto seed=time(0);
        cout<<"Run with seed:"<<seed<<endl;

        srand(seed);
        auto t_start=chrono::high_resolution_clock::now();
        int mbs=0;
        int i=0;
        int totMov=0;
        actionT ris[256];
        char strBuff[20];

        int rt;
        try{
        i=0;
        for(rt=0;rt<20;rt++){
            EBoard* v;
            v=base_state(7);
            i=0;
            while(1){
                int numAction=0;
                for(int i=0;i<MAX_ACTIONS_SIZE;i++)

                getActions(v,ris);
                int m=rand()%ris[0]+1;
                //m=1;
                actionToString(ris[m],v,strBuff);
                cout<<strBuff<<endl;
                actionT na=stringToAction(v,strBuff);
                // if(na!=ris[m]) throw "Different value for action";
                next_state(v,na);
               

                if(checkStatus(v)!=1){
                    cout<<"X"<<endl;
                    break;
                }


                totMov++;
                i++;
            }
         
        }
        auto t_end=chrono::high_resolution_clock::now();  
        cout<<"Tempo totale mossa  "<<chrono::duration<double,milli>(t_end-t_start).count()/1000.0/((double)totMov)<<endl;
            cout<<"Mosse eseguite:"<<totMov<<endl;
        }catch (exception e){
            cout<<"ERRORE :"<<e.what()<<endl;
            cout<<"More info : i="<<i<<", rt="<<rt<<", seed="<<seed<<endl;
        }
    }

    }catch(string e){
        cout<<e<<endl;
    }

}


/*

More info : i=609, rt=0, seed=1745452058
Run with seed:1745452058
ERRORE :std::exception
*/