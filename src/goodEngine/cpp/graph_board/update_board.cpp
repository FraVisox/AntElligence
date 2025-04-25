
#include "update_board.h"

#include "graph_board.h"
#include "build_from_graph.h"


void UpdateState(boardT currentBoard, actionT action){
    int currentBug=action&(0xff);  
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
    pieceT p=(actionT)(act&0xff);
    b.prevMoved[b.currentColor()]=p;
    b.currentTurn++;
    if(act==0)return;
    if(!b.G.isPlaced[p]){
        b.G.isPlaced.set(p,1);
        b.inHandPiece.set(p,0);
    }
    UpdateGameboard(b.G,act);
}

void UpdateGameboard(gameboard &g, actionT act){
    pieceT p=(char)(act&0xff);
    if(act==p){
        g.addPiece(position{0,0},p);
        return;
    }
    // rimuovo vecchio pezzo
    if(g.bugPosition[p]!=NULL_POSITION){
        g.removePiece(p);
    }
    for(int i=0;i<6;i++){
        if(((act>>(8*(i+1)))&(0xff))!=0){
            pieceT ob=(act>>(8*(i+1)))&(0xff);
            position nbPos=g.getPosition(ob);
            position myPosition=nbPos.applayMove((opposite(i)));
            g.addPiece(myPosition,p);
            return;
        }
    }
    
    pieceT bugUnder=(act>>56)&(0xff);
    position pos=g.getPosition(bugUnder);
    g.addPiece(pos,p);
}


void  getActionsWithImplicitTransiction(boardT state,Board& b,actionT* ris){ 
    b.ComputePossibleMoves();
    
    if(b.numAction==0){
        ris[0]=1;
        ris[1]=0;
        return;
    }
    

    for(int j=0;j<=b.numAction;j++){
        ris[j]=0;
    }
    ris[0]=b.numAction;

    for(int i=1;i<=b.numAction;i++){
        //action act=b.resAction[i-1];
        ris[i]=b.resAction[i-1];
        /*if(act.actType==PLACEFIRST)continue;
        position destPos=b.G.getPosition(act.otherBug).applayMove(act.relativeDir);
        int resHight=b.G.getHight(destPos);

        for(int j=0;j<6;j++){
            position posNearJ=destPos.applayMove(numberToDirection(j));
            if(b.G.getHight(posNearJ)>resHight){
                pieceT nextBug=b.G.gb[(posNearJ.first+SIZE_BOARD)%SIZE_BOARD][(posNearJ.second+SIZE_BOARD)%SIZE_BOARD][resHight];
                if(nextBug!=act.bug)
                    ris[i]|=((actionT)(nextBug))<<((actionT)(8*(j+1)));
            }
        }

        if(state[getStartingPointBug(((actionT)(act.bug)))+6]!=0)
            throw "Moving piece with someone on topo";
        if(resHight!=0){
            ris[i]|=((actionT)(((actionT)(b.G.topPiece(destPos)))))<<(56);
        }
        */
        //checkActionValid(state,ris[i]);
    }
    //DEBUG ONLY

}
