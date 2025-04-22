
#ifndef BUILD_FROM_GRAPH_CPP
#define BUILD_FROM_GRAPH_CPP
#include "../engine/board.h"
#include "graph_board.h"
#include "build_from_graph.h"

int seen[32];

void DFSPlace(boardT board, gameboard& gb, position pos, pieceT n){
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
            DFSPlace(board,gb,pos.applayMove(numberToDirection(i)),board[i+startPos]);
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
    
    int sb=0;
    for(int i=1;i<=28;i++){
        if(isPresent(board,i)&& !isPlaced(board,i)){
            g.bugPosition[i]=NULL_POSITION;
        }
        if(isPlaced(board,i)){
            sb=i;
        }
    }
    if(sb==0){
        return g;
    }
    for(int i=0;i<32;i++){
        seen[i]=0;
    }
    DFSPlace(board,g,position{0,0},sb);
    return g;
}


Board buildBoardFromGraph(boardT board){
    Board b;
    b.currentTurn=currentTurn(board);
    b.state=GameState::IN_PROGRESS;
    b.G=buildGameBoardFromGraph(board);
    for(int i=1;i<=28;i++){
        if(isPlaced(board,i)){
            b.G.isPlaced.set(i,1);
        }
        if(isInHand(board,i)){
            b.inHandPiece.set(i,1);
        }
    }

    return b;
}


#endif