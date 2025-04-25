#include "embadded_board.h"
#include "graph_board/build_from_graph.h"
#include "graph_board/update_board.h"

EBoard::EBoard(GameType gt){

    
    for(int i=0;i<BOARDSIZE;i++)graph_board[i]=0;

    setTurn(graph_board,1);
    for(int i=1;i<12;i++){
        addPieceInHand(graph_board,i);
    }
    for(int i=15;i<26;i++){
        addPieceInHand(graph_board,i);
    }
    switch (gt)
    {
    case GameType::Base: break;
    case GameType::Base_M: addPieceInHand(graph_board,12);addPieceInHand(graph_board,26);break;
    case GameType::Base_L: addPieceInHand(graph_board,13);addPieceInHand(graph_board,27);break;
    case GameType::Base_P: addPieceInHand(graph_board,14);addPieceInHand(graph_board,28);break;
    case GameType::Base_ML: addPieceInHand(graph_board,12);addPieceInHand(graph_board,26);
                            addPieceInHand(graph_board,13);addPieceInHand(graph_board,27);break;
    case GameType::Base_MP: addPieceInHand(graph_board,12);addPieceInHand(graph_board,26);
                            addPieceInHand(graph_board,14);addPieceInHand(graph_board,28);break;
    case GameType::Base_LP: addPieceInHand(graph_board,13);addPieceInHand(graph_board,27);
                            addPieceInHand(graph_board,14);addPieceInHand(graph_board,28);break;
    case GameType::Base_MLP:addPieceInHand(graph_board,12);addPieceInHand(graph_board,26);
                            addPieceInHand(graph_board,13);addPieceInHand(graph_board,27);
                            addPieceInHand(graph_board,14);addPieceInHand(graph_board,28);break;
    }

    board_exp=buildBoardFromGraph(graph_board);
}

void EBoard::applyAction(actionT action){
    board_exp.applayAction(action);
    UpdateState(graph_board,action);
}

void EBoard::getNextsActions(actionT* ris){
    getActionsWithImplicitTransiction(graph_board,board_exp,ris);
}

EBoard::EBoard(EBoard* b){
    for(int i=0;i<284;i++){
        this->graph_board[i]=b->graph_board[i];
    }
    this->board_exp.copy(b->board_exp);
}

int EBoard::getState(){
    return checkWin(graph_board);
}

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
            position posBase=board_exp.G.getPosition(i);
            if(board_exp.G.gb[(posBase.first+SIZE_BOARD)%SIZE_BOARD][(posBase.second+SIZE_BOARD)%SIZE_BOARD][0]!=i)continue;

            for(int j=0;j<6;j++){
                if(graph_board[of+j]!=0 && board_exp.currentTurn>2){
                    pieceT otherBug=graph_board[of+j];
                    position otherPos=board_exp.G.getPosition(otherBug);
                    if(posBase.applayMove(j)!=otherPos){
                        throw "Not the correct position";
                    }
                }
            }

            
        }
    }
    
}