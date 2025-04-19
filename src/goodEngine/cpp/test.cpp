#include "embadded_board.h"
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
    cout<<"Run with seed:"<<seed;
    srand(seed);

    int mbs=0;
    int i=0;
    int totMov=0;
    actionT* ris[256];
    for(int rt=0;rt<1000;rt++){
        EBoard eb(GameType::Base);
        i=0;
        while(1){
            eb.getNextsActions(ris);

            int m=rand()%ris[0]+1;
            eb.applyAction(ris[m]);
            if(eb.getState()!=1){
                cout<<i<<endl;
                break;
            }
            eb.checkConsistency();
            i++;
        }
    }
    cout<<(mbs);
}