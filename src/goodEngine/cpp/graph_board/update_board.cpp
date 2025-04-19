
#include "update_board.h"



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


void UpdateBoardE(Board &b,boardT prev, actionT act){
    b.currentTurn++;
    if(act==0)return;
    pieceT p=(actionT)(act&0xff);
    if(!isPlaced(prev,p)){
        b.placedBug.push_back(p);
        b.inHandPiece.extract(p);
        if(p==8)b.isPlacedWQ=1;
        if(p==22)b.isPlacedBQ=1;
    }
    UpdateGameboard(b.G,act,prev);
}

void UpdateGameboard(gameboard &g, actionT act,boardT prev){
    pieceT p=(char)(act&0xff);
    if(currentTurn(prev)==1){
        g.addPiece(position{0,0},p);
        return;
    }
    // rimuovo vecchio pezzo
    if(isPlaced(prev,p)){
        g.removePiece(p);
    }
    for(int i=0;i<6;i++){
        if(((act>>(8*(i+1)))&(0xff))!=0){
            pieceT ob=(act>>(8*(i+1)))&(0xff);
            position nbPos=g.getPosition(ob);
            position myPosition=nbPos.applayMove(numberToDirection(opposite(i)));
            g.addPiece(myPosition,p);
            return;
        }
    }
    
    pieceT bugUnder=(act>>56)&(0xff);
    position pos=g.getPosition(bugUnder);
    g.addPiece(pos,p);
}


void  getActionsWithImplicitTransiction(boardT state,Board& b,actionT* ris){ 
    vector<action> r=b.possibleMoves();
    
    if(r.size()==0){
        ris[0]=1;
        ris[1]=0;
        return;
    }
    

    for(int j=0;j<=(int)r.size();j++){
        ris[j]=0;
    }
    ris[0]=r.size();

    for(int i=1;i<=(int)r.size();i++){
        action act=r[i-1];
        ris[i]=(actionT)act.bug;
        position destPos=b.G.getPosition(act.otherBug).applayMove(act.relativeDir);
        if(act.actType==PLACEFIRST)continue;
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
        //checkActionValid(state,ris[i]);
    }
    //DEBUG ONLY

}
