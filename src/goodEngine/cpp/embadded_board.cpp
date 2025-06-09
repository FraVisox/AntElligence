#include "embadded_board.h"

EBoard::EBoard(GameType gt) : board_exp(gt){}

void EBoard::applyAction(actionT action){
    board_exp.applayAction(action);
}

void EBoard::getNextsActions(actionT* ris){
    board_exp.ComputePossibleMoves();
    
    if(board_exp.numAction==0){
        ris[0]=1;
        ris[1]=0;
        return;
    }   

    ris[0]=board_exp.numAction;

    for(int i=1;i<=board_exp.numAction;i++){
        ris[i]=board_exp.resAction[i-1];
    }
}

EBoard::EBoard(EBoard* b){
    this->board_exp.copy(b->board_exp);
}

int EBoard::getState(){
    return board_exp.getGameState();
}

/*

void EBoard::checkConsistency(){
    return ;
    checkBoardCoherent(graph_board);
    
    for(int i=1;i<=28;i++){
        if(isPlaced(graph_board,i) && !board_exp.G.isPlaced[i]){
            int k=0;
            k+=1;
            throw "Placed a piece not there (yes in Graph)";
        }
        if(!isPlaced(graph_board,i) && board_exp.G.isPlaced[i])
        {
            throw "Placed a piece not there (yes in Graph)";
        }
        if(isPlaced(graph_board,i)){
            int of=getStartingPointBug(i);
            positionT posBase=board_exp.G.getPosition(i);
            if(board_exp.G.gb[0][posBase]!=i)continue;

            for(int j=0;j<6;j++){
                if(graph_board[of+j]!=0 && board_exp.currentTurn>2){
                    pieceT otherBug=graph_board[of+j];
                    positionT otherPos=board_exp.G.getPosition(otherBug);
                    if(applayMove(posBase,j)!=otherPos){
                        throw "Not the correct position";
                    }
                }
            }
            
        }
    }
    
}
*/