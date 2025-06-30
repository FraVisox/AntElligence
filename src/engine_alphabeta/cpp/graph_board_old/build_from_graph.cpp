
#ifndef BUILD_FROM_GRAPH_CPP
#define BUILD_FROM_GRAPH_CPP
#include "../engine/board.h"
#include "graph_board.h"
#include "build_from_graph.h"

int seen[32];

void DFSPlace(boardT board, gameboard& gb, positionT pos, pieceT n){
    if(seen[n])return;
    seen[n]=1;
    gb.addPiece(pos,n);
    //gb.bugPosition.insert(make_pair(n,pos));
    //gb.occupied.insert(pos);
    //int currH=gb.getHight(pos);
    //gb.gb[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD][currH]=(n);
    //gb.high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD]++;

    int startPos=getStartingPointBug(n);
    for(int i=0;i<6;i++){
        if(board[i+startPos]!=0){
            DFSPlace(board,gb,applayMove(pos,(i)),board[i+startPos]);
        }
    }

    while(board[6+getStartingPointBug(n)]!=0){
        n=board[6+getStartingPointBug(n)];
        gb.addPiece(pos,n);
        seen[n]=1;
        //gb.bugPosition.insert(make_pair(n,pos));
        //gb.gb[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD][gb.getHight(pos)]=(n);
        //gb.high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD]++;
    }
}

gameboard buildGameBoardFromGraph(boardT board){ 
    gameboard g;
    
    return g;
}


Board buildBoardFromGraph(boardT board){
}


#endif