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
    auto seed=time(0);
    cout<<"Run with seed:"<<seed<<endl;
    srand(seed);
    auto t_start=chrono::high_resolution_clock::now();
    int mbs=0;
    int i=0;
    int totMov=0;
    actionT ris[256];
    EBoard eb(GameType::Base);
    i=0;
    eb.getNextsActions(ris);
    cout<<"NO1"<<endl;
    eb.applyAction(ris[1]);
    cout<<"NO2"<<endl;
    eb.getNextsActions(ris);
    cout<<"NO3"<<endl;
    cout<<ris[0]<<endl;
    for(int i=1;i<=ris[0];i++){
        cout<<ris[i]<<endl;
    }

    cout<<"NOENDO";
        
    /*
    for(int rt=0;rt<10000;rt++){
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
            if(eb.getState()!=1){
                totMov+=i;
                break;
            }
            //eb.checkConsistency();
            i++;
        }
        if(totMov>1000000){

            auto t_end=chrono::high_resolution_clock::now();  
            cout<<"Tempo totale mossa  "<<chrono::duration<double,milli>(t_end-t_start).count()/((double)totMov)<<endl;
            break;
        }
    }
+7*/
    cout<<(mbs);
}


// LOOK SEED seed:1745067833