#include "graph_board.h"
#include "build_from_graph.h"
#include "engine/board.h"
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
    char board[2][BOARDSIZE];
    for(int i=0;i<BOARDSIZE;i++){
        board[0][i]=0;
    }

    setTurn(board[0],1);
    for(int i=1;i<12;i++){
        addPieceInHand(board[0],i);
    }
    for(int i=15;i<26;i++){
        addPieceInHand(board[0],i);
    }


    for(int rt=0;rt<100000;rt++){
    for(int i=0;i<40000;i++){
        int64_t* ris=getActionsWithExplicitTransiction(board[i%2]);
        
        int m=rand()%ris[0]+1;
        
        
        findNextState(board[i%2],board[(i+1)%2],ris[m]);
        //printActionFancy(ris[m]);
        //printBoardFancy(board[(i+1)%2]);
        if(checkWin(board[(i+1)%2])!=1 ){
            if(rt%1000==0){
                cout<<"-";
            cout.flush();
            }
            //cout<<"ENDED"<<endl;
            //printActionFancy(ris[m]);
            //printBoardFancy(board[(i+1)%2]);
            break;
       
        }
        
        free(ris);
    }
    }

}