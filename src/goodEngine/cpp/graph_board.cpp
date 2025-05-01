#include "graph_board.h"


#ifndef GRAPH_BOARD_CPP
#define GRAPH_BOARD_CPP


inline int opposite(int x){
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

void placePiece(boardT board, int n){
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



bool getColor(int n){
    return n/15;  // da 1 a 14 da 0, da 15 a 28 da 1.
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


int bugNumber(int n){
    n=(n-1)%14;
    switch (n)
    {
    case 0:return 1;    // S1
    case 1:return 2;    // S2
    case 2:return 1;    // B1
    case 3:return 2;    // B2
    case 4:return 1;    // G1
    case 5:return 2;    // G2
    case 6:return 3;    // G3
    case 7:return 0;    // Q
    case 8:return 1;    // A1
    case 9:return 2;    // A2
    case 10:return 3;   // A3
    case 11:return 0;   // M
    case 12:return 0;   // L
    case 13:return 0;   // P
    default: 
        throw "Non è un bug, cosa accade???";
    }
}

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


    placePiece(nextBoard,currentBug);

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
        nextBoard[nSP+opposite(7)]=currentBug;
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

void checkActionValid(boardT s, actionT a){
    boardT r=(char*)malloc(sizeof(char)*BOARDSIZE);
    findNextState(s,r,a);

    checkConnected(r);




    free(r);
}


int64_t*  getActionsWithExplicitTransiction(boardT state){
    Board b=buildBoardFromGraph(state);
    
    vector<action> r=b.possibleMoves();
    
    if(r.size()==0){
        int64_t* ris= (int64_t*)malloc(2+1);
        ris[0]=1;
        ris[1]=0;
        return ris;
    }
    int64_t* ris=(int64_t*)malloc(sizeof(int64_t)*r.size()+1);


    for(int j=0;j<=r.size();j++){
        ris[j]=0;
    }
    ris[0]=r.size();

    for(int i=1;i<=r.size();i++){
        action act=r[i-1];
        ris[i]=PieceToNum(act.bug);
        position destPos=b.G.getPosition(act.otherBug).applayMove(act.relativeDir);
        if(act.actType==PLACEFIRST)continue;
        int resHight=b.G.getHight(destPos);

        for(int j=0;j<6;j++){
            position posNearJ=destPos.applayMove(numberToDirection(j));
            if(b.G.getHight(posNearJ)>resHight){
                piece nextBug=b.G.gb[(posNearJ.first+SIZE_BOARD)%SIZE_BOARD][(posNearJ.second+SIZE_BOARD)%SIZE_BOARD][resHight];
                if(nextBug!=act.bug)
                    ris[i]|=PieceToNum(nextBug)<<(8*(j+1));
            }
        }

        if(state[getStartingPointBug(PieceToNum(act.bug))+6]!=0)
            throw "Moving piece with someone on topo";
        if(resHight!=0){
            ris[i]|=((int64_t)(PieceToNum(b.G.topPiece(destPos))))<<(56);
        }
       // checkActionValid(state,ris[i]);
    }
    //DEBUG ONLY
    return ris;

}




void printBoardFancy(boardT board){
    cout<<"Turn: "<<currentTurn(board)<<"   Status:"<<checkWin(board)<<endl;

    for(int i=1;i<=28;i++){
        cout<<NumToPiece(i).toString()<<"|"<<isPresent(board,i)<<"|"<<isPlaced(board,i)<<"||";
        int x=getStartingPointBug(i);
        for(int i=0;i<8;i++){
            if((int)board[i+x]==0){
                cout<< "   |";
            }else{

                cout<<NumToPiece((int)board[i+x]).toString()<<"|";
            }
        }
        cout<<endl;
    }
    
}



void printActionFancy(actionT a){

    
    cout<<"Act: "<<NumToPiece((a&0xff)).toString()<<"|";

    for(int i=1;i<7;i++){
        if(( (a>>(8*i))&(0xff))==0)cout<<"   |";
        else cout<<NumToPiece(( (a>>(8*i))&(0xff))).toString()<<"|";
    }
    cout<<( (a>>(56))&(0xff)) <<"|"<< endl;
}






















/*

vector<piece> placedBug = vector<piece>();
unordered_set<piece> inHandPiece = unordered_set<piece>();

    //Initialization

    Board();

    void reset();
    void copy(const Board& b);

    //Current status
    string toString();
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();

    //Make movement
    ReturnMessage executeAction(string s);
    action validMove(string s);
    ReturnMessage checkWin();
    bool checkSurrounding(const piece &p);

    void undo(int amount);

    void addPieceHand(piece p);

    vector<action> possibleMoves();
    void possibleMovesBug(piece b, vector<action> &res);
    void possibleMoves_Queen(piece bug,vector<action> &res);
    void possibleMoves_Beetle(piece bug,vector<action> &res);
    void possibleMoves_Grasshopper(piece bug,vector<action> &res);
    void possibleMoves_SoldierAnt(piece bug, vector<action> & res);
    void possibleMoves_Spider(piece bug, vector<action> & res);
    void possibleMoves_Pillbug(piece bug, vector<action> &res);
    void possibleMoves_Mosquito(piece bug, vector<action> &res);
    void possibleMoves_Ladybug(piece bug,vector<action> &res);
};


#endif

*/


#endif