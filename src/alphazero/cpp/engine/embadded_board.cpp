#include "embadded_board.h"

#include <iostream>
using namespace std;

EBoard::EBoard(GameType gt) : board_exp(gt){}

void EBoard::applyAction(actionT action){
    //cout<<"Applay actions"<<action<<endl;
    board_exp.applayAction(action);
}

void EBoard::getNextsActions(actionT* ris){
    if(getState()>1){
        cout<<"Asking to expand a finished state, not possible"<<endl;
        throw "Not expandible";
    }
    
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


void EBoard::updateVectRapp(){
    bitset<32> occR(0),occC(0);
    gameboard &gb=this->board_exp.G;
    for(int i=0;i<32;i++){
        if(gb.isPlaced[i]){
            int p=gb.bugPosition[i];
            occR.set(p>>5,1);
            occC.set(p&31,1);
        }
    }
    //cout << "A" << endl;

    positionT fr=0;
    while(occR[fr]&& fr<32)fr++;
    while((!occR[fr+1]) && fr<31)fr++;
    positionT fc=0;
    while(occC[fc]&& fc<32)fc++;
    while((!occC[fc+1]) && fc<31)fc++;
    //cout << "A" << endl;
    
    
    positionT bp=(fr<<5)+fc;

    for(int i=0;i<1024;i++){
        this->vectRapp[i]=(gb.isFree((i+bp)&1023))
                            ?0
                            :(gb.topPiece((i+bp)&1023));
    }
    //cout << "A" << endl;

    // put climber pieces
    int climberPiece[]={3,4,12,17,18,26};
    //cout << "A" << endl;

    for(int i=0;i<6;i++){
        pieceT cp=climberPiece[i];
        this->vectRapp[1024+i]=0;
        if(gb.isPlaced[cp]){
            positionT pos=gb.getPosition(climberPiece[i])&1023;
            int h=0;
            while(gb.gb[h][pos]!=cp)h++;
            if(h!=0){
                this->vectRapp[1024+i]=gb.gb[h-1][pos];
            }
        }
    }
    //cout << "A" << endl;
    

    this->vectRapp[1030]=this->board_exp.prevMoved[0];
    this->vectRapp[1031]=this->board_exp.prevMoved[1];
    this->vectRapp[1032]=this->board_exp.currentTurn;
    //cout << "A" << endl;
    
}


int EBoard::getTurn(){
    return board_exp.currentTurn;
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

