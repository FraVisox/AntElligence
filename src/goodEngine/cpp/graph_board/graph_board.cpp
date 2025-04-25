#include "graph_board.h"
#include "update_board.h"
#include "build_from_graph.h"

#ifndef GRAPH_BOARD_CPP
#define GRAPH_BOARD_CPP


int opposite(int x){
    if(x<6)
        return (x+3)%6;
    return 13-x;
}

int getStartingPointBug(int bugNumber ){
    return (bugNumber-1)*10+INFOBOARD_SIZE;
}

bool isPlaced(boardT board, int n){
    int x=getStartingPointBug(n);
    return board[x+8]&1;
}

void placePieceG(boardT board, int n){
    int x=getStartingPointBug(n);
    board[x+8]|=1;
}

void addPieceInHand(boardT board, int n){
    int x=getStartingPointBug(n);
    board[x+8]|=10;
}

bool isPresent(boardT board, int n){
    int x=getStartingPointBug(n);
    return board[x+8]&10;
}

bool isInHand(boardT board, int n){
    return isPresent(board,n) && !isPlaced(board, n);
}





/* la board sarà una sequenza di 28*10 short

- n1
- n2
- ..
- n6
- over
- under
-  info*2
- 
genInfo : 4

infoA -> 8
.....  | ... | ... | isPlaced | isPresent


action: sequenza di 9 byte



    0    1  
 
  5        2

    4    3



in grafico


p.     | 0 |  1              |(i+1,j)|(i+1,j+1)
d.  5  |i,j|  2     (i  ,j-1)|[i,j]  |(i  ,j+1)
p.     | 4 |  3              |(i-1,j)|(i-1,j+1)


 i
 |
\ /
d.  0  | 1 |        (i+1,j-1)|(i+1,j)|
p.  4  |i,j|  2     (i  ,j-1)|[i,j]  | (i,j+1)
d.  5  | 3 |        (i-1,j-1)|(i-1,j)|




|b_init|v1, v2, ... v9|

*/



int currentTurn(boardT b){
    return (int)(u_char)b[TURNPOS]*256+(int)(u_char)b[TURNPOS+1];
}


void setTurn(boardT b,int n){   
    b[TURNPOS]=(n/256)%256;
    b[TURNPOS+1]=n%256;
}

int checkWin(boardT b){
    int bugWQ=0;
    int startingPointWQ=getStartingPointBug(8);
    for(int i=0;i<6;i++){
        if(b[i+startingPointWQ]!=0)bugWQ++;
    }
    if(bugWQ==6)return 3;
    int bugBQ=0;
    int startingPointBQ=getStartingPointBug(22);
    for(int i=0;i<6;i++){
        if(b[i+startingPointBQ]!=0)bugBQ++;
    }
    if(bugBQ==6)return 4;
    if(currentTurn(b)==0) return 0;
    return 1;
}





void findNextState(boardT currentBoard,boardT nextBoard, actionT action){
    int currentBug=action&(0xff);  
    if(currentBug==0) return;

    for(int i=0;i<BOARDSIZE;i++){
        nextBoard[i]=currentBoard[i];
    }


    placePieceG(nextBoard,currentBug);

    nextBoard[TURNPOS+1]+=1;
    if(nextBoard[TURNPOS+1]==0)nextBoard[TURNPOS]+=1;
    
    
    int bugStarting=getStartingPointBug(currentBug);
    for(int i=0;i<8;i++){  // rimuovo pezzo da dove era
        if(currentBoard[bugStarting+i]!=0){
            char neigbour=currentBoard[bugStarting+i];
            nextBoard[getStartingPointBug(neigbour)+opposite(i)]=0;
        }
    }

    for(int i=0;i<6;i++){  // aggiorno con nuovi vicini
        if(((action>>(8*(i+1)))&(0xff))!=0){
            int nSP=getStartingPointBug((action>>(8*(i+1)))&(0xff));
            nextBoard[nSP+opposite(i)]=currentBug;
        }
    }
    if(((action>>56)&(0xff))!=0){
        int nSP=getStartingPointBug((action>>56)&(0xff));
        nextBoard[nSP+6]=currentBug;
    }

    for(int i=0;i<6;i++){  // aggiorno con nuovi vicini
        nextBoard[bugStarting+i]=(action>>(8*(i+1)))&(0xff);
    }
    if(currentBoard[bugStarting+6]!=0)
        throw "Moving piece with someone on topo";
    nextBoard[bugStarting+6]=0;  
    nextBoard[bugStarting+7]=(action>>56)&(0xff);
    
}





bool seenDBG[32];

void dfsDBG(boardT s, int i){
    if(seenDBG[i])return;
    seenDBG[i]=1;
    int ps=getStartingPointBug(i);

    for(int j=0;j<8;j++){
        if(s[ps+j]!=0){
            dfsDBG(s,s[ps+j]);
        }
    }
    
}
void checkConnected(boardT s){
    for(int i=0;i<32;i++)seenDBG[i]=0;
    
    int i=1;
    while (!isPlaced(s,i))i++;
    dfsDBG(s,i);
    for(int i=1;i<=28;i++)
        if((isPlaced(s,i)&&!seenDBG[i])|| (!isPlaced(s,i)&&seenDBG[i])){
            throw "NOT CONNECETD AFTER ACTION";
        }
    
}
void checkBoardCoherent(boardT s){
    for(int i=1;i<=28;i++){
        int bp=getStartingPointBug(i);
        for(int j=0;j<8;j++){
            if(s[bp+j]!=0){
                if(s[getStartingPointBug(s[bp+j])+opposite(j)]!=i){
                    throw "Non coerent correlation";
                }
            }
        }
    }
}

void checkActionValid(boardT s, actionT a){
    boardT r=(char*)malloc(sizeof(char)*BOARDSIZE);
    findNextState(s,r,a);

    checkConnected(r);

    checkBoardCoherent(r);
    free(r);
}


void  getActionsWithExplicitTransiction(boardT state, actionT* ris){
    Board b=buildBoardFromGraph(state);
    getActionsWithImplicitTransiction(state,b,ris);

}




void printBoardFancy(boardT board){
    cout<<"Turn: "<<currentTurn(board)<<"   Status:"<<checkWin(board)<<endl;

    for(int i=1;i<=28;i++){
        cout<<PiecetoString(i)<<"|"<<isPresent(board,i)<<"|"<<isPlaced(board,i)<<"||";
        int x=getStartingPointBug(i);
        for(int i=0;i<8;i++){
            if((int)board[i+x]==0){
                cout<< "   |";
            }else{

                cout<<PiecetoString((int)board[i+x])<<"|";
            }
        }
        cout<<endl;
    }
    
}



void printActionFancy(actionT a){

    
    cout<<"Act: "<<PiecetoString((a&0xff))<<"|";

    for(int i=1;i<7;i++){
        if(( (a>>(8*i))&(0xff))==0)cout<<"   |";
        else cout<<PiecetoString(( (a>>(8*i))&(0xff)))<<"|";
    }
    cout<<PiecetoString( (a>>(56))&(0xff)) <<"|"<< endl;
}

void printBG(gameboard &g){
    for(int i=0;i<SIZE_BOARD;i++){
        for(int q=0;q<i;q++)cout<<"  ";
        for(int j=0;j<SIZE_BOARD;j++){
            if(g.high[(i-16+SIZE_BOARD)%SIZE_BOARD][(j-16+SIZE_BOARD)%SIZE_BOARD]==0){
                cout<<"   ";
            }
            if(g.high[(i-16+SIZE_BOARD)%SIZE_BOARD][(j-16+SIZE_BOARD)%SIZE_BOARD]>1){
                cout<<"XXX";
            }
            if(g.high[(i-16+SIZE_BOARD)%SIZE_BOARD][(j-16+SIZE_BOARD)%SIZE_BOARD]==1){
                cout<<PiecetoString(g.gb[(i-16+SIZE_BOARD)%SIZE_BOARD][(j-16+SIZE_BOARD)%SIZE_BOARD][0]);
            }
            cout<<"|";
        }
        cout<<endl;
    }
}











#endif