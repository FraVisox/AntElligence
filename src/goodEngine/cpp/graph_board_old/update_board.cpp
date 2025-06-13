
#include "update_board.h"

#include "graph_board.h"
#include "build_from_graph.h"


void UpdateState(boardT currentBoard, actionT action){
    int currentBug=action&(31);  
    if(currentBug==0) return;

    placePieceG(currentBoard,currentBug);

    currentBoard[TURNPOS+1]+=1;
    if(currentBoard[TURNPOS+1]==0)currentBoard[TURNPOS]+=1;
    
    
    int bugStarting=getStartingPointBug(currentBug);
    for(int i=0;i<8;i++){  // rimuovo pezzo da dove era
        if(currentBoard[bugStarting+i]!=0){
            char neigbour=currentBoard[bugStarting+i];
            currentBoard[getStartingPointBug(neigbour)+opposite(i)]=0;
        }
    }

    for(int i=0;i<6;i++){  // aggiorno con nuovi vicini
        if(((action>>(8*(i+1)))&(0xff))!=0){
            int nSP=getStartingPointBug((action>>(8*(i+1)))&(0xff));
            currentBoard[nSP+opposite(i)]=currentBug;
        }
    }
    if(((action>>56)&(0xff))!=0){
        int nSP=getStartingPointBug((action>>56)&(0xff));
        currentBoard[nSP+6]=currentBug;
    }

    for(int i=0;i<6;i++){  // aggiorno con nuovi vicini
        currentBoard[bugStarting+i]=(action>>(8*(i+1)))&(0xff);
    }
    currentBoard[bugStarting+6]=0;  
    currentBoard[bugStarting+7]=(action>>56)&(0xff);
}


void UpdateBoardE(Board &b, actionT act){
    }

void UpdateGameboard(gameboard &g, actionT act){
    
}


void  getActionsWithImplicitTransiction(boardT state,Board& b,actionT* ris){ 

}
