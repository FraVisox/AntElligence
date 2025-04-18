
#ifndef BUILD_FROM_GRAPH_CPP
#define BUILD_FROM_GRAPH_CPP
#include "engine/board.h"
#include "graph_board.h"
#include "build_from_graph.h"

int seen[32];

void DFSPlace(boardT board, gameboard& gb, position pos, int n){
    if(seen[n])return;
    seen[n]=1;
    gb.bugPosition.insert(make_pair(NumToPiece(n),pos));
    gb.occupied.insert(pos);
    int currH=gb.getHight(pos);
    gb.gb[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD][currH]=(NumToPiece(n));
    gb.high[(pos.first+SIZE_BOARD)%SIZE_BOARD][(pos.second+SIZE_BOARD)%SIZE_BOARD]++;

    int startPos=getStartingPointBug(n);
    for(int i=0;i<6;i++){
        if(board[i+startPos]!=0){
            DFSPlace(board,gb,pos.applayMove(numberToDirection(i)),board[i+startPos]);
        }
    }
    if(board[6+startPos]!=0){
        DFSPlace(board,gb,pos,board[6+startPos]);
    }   
}

gameboard buildGameBoardFromGraph(boardT board){ 
    gameboard g;
    g.reset();
    int sb=0;
    for(int i=1;i<=28;i++){
        if(isPresent(board,i)&& !isPlaced(board,i)){
            g.bugPosition.insert(make_pair(NumToPiece(i),NULL_POSITION));
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

piece NumToPiece(int n){
    piece p(0);
    if(getColor(n)==0){
        p.col=PlayerColor::WHITE;
    }else{
        p.col=PlayerColor::BLACK;
    }
    p.numIncr=bugNumber(n);

    n=(n-1)%14;
    switch (n)
    {
    case 0: p.kind=BugType::SPIDER;break;
    case 1: p.kind=BugType::SPIDER;break;
    case 2: p.kind=BugType::BEETLE;break;
    case 3: p.kind=BugType::BEETLE;break;
    case 4: p.kind=BugType::GRASSHOPPER;break;
    case 5: p.kind=BugType::GRASSHOPPER;break;
    case 6: p.kind=BugType::GRASSHOPPER;break;
    case 7: p.kind=BugType::QUEEN;break;
    case 8: p.kind=BugType::SOLDIER_ANT;break;
    case 9: p.kind=BugType::SOLDIER_ANT;break;
    case 10: p.kind=BugType::SOLDIER_ANT;break;
    case 11: p.kind=BugType::MOSQUITO;break;
    case 12: p.kind=BugType::LADYBUG;break;
    case 13: p.kind=BugType::PILLBUG;break;
    }
    return p;
}

Board buildBoardFromGraph(boardT board){
    Board b;
    b.currentTurn=currentTurn(board);
    b.state=GameState::IN_PROGRESS;
    b.G=buildGameBoardFromGraph(board);
    for(int i=1;i<=28;i++){
        if(isPlaced(board,i)){
            b.placedBug.push_back(NumToPiece(i));
        }
        if(isInHand(board,i)){
            b.inHandPiece.insert(NumToPiece(i));
        }
    }

    b.isPlacedWQ=(isPlaced(board,8));
    b.isPlacedBQ=(isPlaced(board,22));
    return b;
}


actionT PieceToNum(piece p){
    int64_t n=0;

    switch (p.kind)
    {
    case BugType::SPIDER:
        n=p.numIncr-1;  // 0-1
        break;
    case BugType::BEETLE:
        n=p.numIncr+1;  // 2-3
        break;
    case BugType::GRASSHOPPER:
        n=p.numIncr+3;  //4-5-6
        break;
    case BugType::QUEEN:
        n=7;
        break;
    case BugType::SOLDIER_ANT:
        n=p.numIncr+7;  // 8-9-10;
        break;
    case BugType::MOSQUITO:
        n=11;
        break;
    case BugType::LADYBUG:
        n=12;
        break;
    case BugType::PILLBUG:
        n=13;
        break;
    }

    if(p.col==BLACK){
        return n+15;
    }
    return n+1;
}

#endif