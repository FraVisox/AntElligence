#include "board.h"
#include <algorithm>

#include <iostream>
#ifndef BOARD_CPP
#define BOARD_CPP



/**
 * \brief Construct a new Board, initializing all members to default values.
 *
 * Initialize the new Board with all members set to default values.
 * The state of the board is set to NOT_STARTED, indicating that the game has not yet started.
 * The current turn is set to 1, indicating that it is White's turn to play.
 * The possible moves vector is cleared.
 * The game board and pieces are reset using the reset() method.
 * The isPlacedBQ and isPlacedWQ flags are set to false, indicating that neither the
 * black nor white queens have been placed on the board yet.
 */
Board::Board(){ 

    currentTurn = 1;
    //We do not change the type of game
    inHandPiece.reset();
    prevMoved[0]=0;
    prevMoved[1]=0;
};



/*
    QUEEN=0,
    SPIDER=1,
    BEETLE=2,
    GRASSHOPPER=3,
    SOLDIER_ANT=4,
    MOSQUITO=5,
    LADYBUG=6,
    PILLBUG=7
*/

int kindValue[] = {100, 10, 70, 40, 90, 35, 60, 45};

int Board::getScoreBug(pieceT p) {
    int multipl = kindValue[kind(p)];
    if (isPinned(p) || isCovered(p)) {
        return -multipl;
    }
    return multipl*(friendlyNeighbour(p)-enemyNeighbour(p));
}

int Board::getScore(PlayerColor color) {
    int res = 0;
    if (G.isPlaced[22] && G.isPlaced[8]) {
        for (pieceT p=1;p<=28;p++) {
            if(G.isPlaced[p]){
                if (col(p) == color) {
                    res += getScoreBug(p);
                } else {
                    res -= getScoreBug(p);
                }
            }
        }
    }
    return res;
}

/*
STARTING TIPS:

WHITE:

    Turn 1: Place wG1 (already handled)

    Turn 3: Place wQ to form elbow:
    - If opponent is on your right → place Queen on left of wG1
    - If opponent is on your left → place Queen on right of wG1

    Turn 5: Place wA1 toward where you expect enemy Queen

BLACK:

    Turn 2: Place bS1 adjacent to wG1 (already handled)

    Turn 4: Place bQ next to bS1 (form inline or elbow shape)
        - Prefer inline (bQ opposite wG1 from bS1), unless blocked

    Turn 6: Add bA1 near bQ

*/
actionT Board::suggestInitialMove() {
    /*
    if (currentTurn == 3) {
        //based on previous placement of black, decide. It should do an elbow placement.
        direction d = getElbowDir(moves[1].relativeDir);
        //Should place queen on "wG1" with the direction
        string move = "wQ "+insertDirection(moves[0].bug.toString(), d); //todo: non mette la queen
        return parseAction(move, inHandPiece);
    } 

    //based on previous placement of white, decide. It should do an elbow on the other side or in general an elbow placement
    position first = G.getPosition(moves[0].bug);
    position second = G.getPosition(moves[1].bug);
    vector<position> nears = nearBoth(first, second);
    if (countSurrounding(nears[0]) == 3) {
        //Place near nears[1]
        return parseAction("bQ "+insertDirection(moves[1].bug.toString(), getNextDir(G.getPosition(moves[1].bug), nears[1], G.getPosition(moves[0].bug))), inHandPiece);
    } 
    //Place near nears[0]
    return parseAction("bQ "+insertDirection(moves[1].bug.toString(), getNextDir(G.getPosition(moves[1].bug), nears[0], G.getPosition(moves[0].bug))), inHandPiece);
    */
   return 0;
}


int Board::countSurrounding(pieceT p) {
    positionT pos = G.getPosition(p);
    int ret = 0;
    for (direction dir=0;dir<6;dir++){
        positionT adj= applayMove(pos,dir);
        if (!G.isFree(adj)) {
            ret += 1;
        }
    }
    return ret;
}
    


/**
 * \brief Return the color of the current player.
 *
 * \return The color of the current player (WHITE or BLACK).
 */
PlayerColor Board::currentColor(){
    if(currentTurn%2==1) return PlayerColor::WHITE;
    return PlayerColor::BLACK;
}

/**
 * \brief Returns the turn number of the current player.
 *
 * The turn number increases every two moves, so the first move is turn 1, the third move is turn 2, and so on.
 *
 * \return The turn number of the current player.
 */
int Board::currentPlayerTurn(){
    return (currentTurn-1)/2+1;
}

void Board::copy(Board &b){
    this->G.copy(b.G);
    this->currentTurn=b.currentTurn;
    this->inHandPiece=b.inHandPiece;
    this->numAction=b.numAction;
    this->prevMoved[0]=b.prevMoved[0];
    this->prevMoved[1]=b.prevMoved[1];
    for(int i=0;i<256;i++)
        this->resAction[i]=b.resAction[i];

    for(int i=0;i<MAX_ACTIONS_SIZE;i++)
        this->resAction[i]=b.resAction[i];
    //for(int i=0;i<MAX_TURN_SIZE;i++)
    //    this->confHistory[i]=b.confHistory[i];
    for(int i=0;i<20;i++)
        this->pillbugMoves[i]=b.pillbugMoves[i];
    this->pillbugTotMoves=b.pillbugTotMoves;
    this->inQueue=b.inQueue;
}




Board::Board(GameType gt) : G(gt){

    currentTurn=1;
    for(int i=1;i<12;i++){
        addPieceHand(i);
    }
    for(int i=15;i<26;i++){
        addPieceHand(i);
    }
    switch (gt)
    {
        case GameType::Base: break;
        case GameType::Base_M:  addPieceHand(12);addPieceHand(26);break;
        case GameType::Base_L:  addPieceHand(13);addPieceHand(27);break;
        case GameType::Base_P:  addPieceHand(14);addPieceHand(28);break;
        case GameType::Base_ML: addPieceHand(12);addPieceHand(26);
                                addPieceHand(13);addPieceHand(27);break;
        case GameType::Base_MP: addPieceHand(12);addPieceHand(26);
                                addPieceHand(14);addPieceHand(28);break;
        case GameType::Base_LP: addPieceHand(13);addPieceHand(27);
                                addPieceHand(14);addPieceHand(28);break;
        case GameType::Base_MLP:addPieceHand(12);addPieceHand(26);
                                addPieceHand(13);addPieceHand(27);
                                addPieceHand(14);addPieceHand(28);break;
    }
}

/**
 * \brief Checks if the queen for the current player has been placed.
 *
 * \return True if the queen for the current player has been placed, false otherwise.
 */
bool Board::placedQueen(){
    if(currentColor()==WHITE) 
        return G.isPlaced[8];
    return G.isPlaced[22];
}


/**
 * \brief Adds a piece to the in-hand pieces set.
 *
 * This method is used to add a piece to the set of pieces that are
 * currently in the player's hand. The piece is added to the set
 * of in-hand pieces, which is used to determine which pieces can
 * be placed on the board.
 *
 * \param p The piece to add to the in-hand pieces set.
 */

void Board::addPieceHand(pieceT p){
    inHandPiece.set(p,1);
}



void Board::applayAction(actionT act){
    //confHistory[currentTurn]=G.toHash();
    if(act==0){
        currentTurn++;
        prevMoved[currentColor()]=0;
        return;
    }

    pieceT p=(act%32);
    positionT dest=act/32;

    if(!G.isPlaced[p]){
        currentTurn++;
        prevMoved[currentColor()]=0;
        inHandPiece.set(p,0);
        
    }else{
        computePillbugMovinPieces();
        prevMoved[1-currentColor()]=p;
        for(int i=0;i<pillbugTotMoves;i++){
            if(act == pillbugMoves[i]){
                prevMoved[currentColor()]=0;
            }
        }
        currentTurn++;
        G.removePiece(p);
    }
    G.addPiece(dest,p);
}


GameState Board::getGameState(){
    int bugWQ=0,bugBQ=0;

    positionT WQPos=G.getPosition(8);
    positionT BQPos=G.getPosition(22); 
    for(int i=0;i<6;i++){
        if(!G.isFree(applayMove(WQPos,i)))bugWQ++;
        if(!G.isFree(applayMove(BQPos,i)))bugBQ++;
    }    
    if(bugWQ==6 && bugBQ==6)return (GameState)(2+currentColor());
    if(bugBQ==6)return GameState::WHITE_WIN;
    if(bugWQ==6)return GameState::BLACK_WIN;
    
    bitset<285> r=G.toHash();
    int ne=0;
    for(int i=1;i<currentTurn; i++){
        if(r==confHistory[i])
            ne++;
    }  
    
    if(currentTurn==0) return GameState::NOT_STARTED;
    if(currentTurn>MAX_TURN_SIZE || ne>1)return GameState::DRAW;
    return GameState::STARTED;
}


void Board::ComputePossibleMoves(){
    numAction=0;  
    pillbugTotMoves=0;
    // 1 If turn == 1, then place something that is not the queen
    if(currentTurn==1){
        for(int b=1;b<=28;b++){
            if(inHandPiece[b]){
            if(col(b)==currentColor() && kind(b) != QUEEN && (numInc(b) == 0 || numInc(b) == 1)) {                
                resAction[numAction]=placeFirst(b);
                numAction++;
            }
            }
        }
        return;
    }

    // 2 If turn == 2 , then place something near white;
    if(currentTurn == 2){
        for(int  b=0;b<32;b++){
            if(inHandPiece[b]){
            if(col(b)==currentColor() && kind(b) != QUEEN && (numInc(b) == 0 || numInc(b) == 1)){
                for(int dir=0;dir<6;dir++){

                    // SOLVED IN PLACE PIECE
                    resAction[numAction]=placePiece(b,applayMove(0,dir),G);
                    numAction++;
                }
            }
            }
        }
        return;
    }

    bool computedValidPos=false;
    // 3 - If i didn't place the queen after turn 3, it's the only move
    if(!placedQueen() && currentPlayerTurn()>3){
        G.computeValidPositionPlaceNew(currentColor());
        computedValidPos=true;
        for(int nvp=0;nvp<G.numValidPosition;nvp++){
            // SOLVED IN PLACE PIECE
            resAction[numAction]=placePiece(((currentColor()==PlayerColor::WHITE)?8:22),G.validPositionPlaceBuffer[nvp],G);
            numAction++;
        }
        return;
    }
    
    // 4 - Place pieces in our hand
    pieceT inHandCol[8] = {INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE,INVALID_PIECE};
    bool toPlace = false;
    for(int p=0;p<32;p++){
        if(inHandPiece[p]){
            if(col(p)==currentColor()){
                if (inHandCol[kind(p)] == INVALID_PIECE || numInc(inHandCol[kind(p)]) > numInc(p)){ 
                    inHandCol[kind(p)] = p;
                    toPlace = true;
                }
            }
        }
    }
    if(toPlace){
        if(!computedValidPos)
            G.computeValidPositionPlaceNew(currentColor());
        for(auto p:inHandCol) {
            if (p == INVALID_PIECE) {
                continue;
            }
            for(int nvp=0;nvp<G.numValidPosition;nvp++){
                // SOLVED IN PLACE PIECE
                resAction[numAction]=placePiece(p, G.validPositionPlaceBuffer[nvp],G);
                numAction++;
            }
        }
    }
    //cout<<"Move place:"<<numAction<<endl;

    // 5- moves
    if(placedQueen()){
        for(pieceT b=1;b<=28;b++){
            if(!G.isPlaced[b])continue;
            if (b == prevMoved[currentColor()]) continue;
            possibleMovesBug(b);
        }
        computePillbugMovinPieces();
        for(int i=0;i<pillbugTotMoves;i++){
            resAction[numAction]=pillbugMoves[i];
        }
        /*
        if(pillbugTotMoves>0){ 

            bitset<32> seen(0);
            int startPar=0;
            int endPart=1;   // excluded
            int tA=numAction;
            std::sort(pillbugMoves,pillbugMoves+pillbugTotMoves);
        
            while(endPart<=tA){
                while(endPart<tA && resAction[endPart-1]<resAction[endPart]){
                    endPart++;
                }
                int i=startPar,j=0;
                while(i<endPart && j<pillbugTotMoves){
                    if(resAction[i]==pillbugMoves[j]){
                        seen.set(j,1);
                        i++;
                        j++;
                    }else if(resAction[i]<pillbugMoves[j]){
                        i++;
                    }else{
                        j++;
                    }
                }
                startPar=endPart;
                endPart=startPar+1;
            }
            for(int i=0;i<pillbugTotMoves;i++){
                if(!seen[i]){
                    resAction[numAction]=pillbugMoves[i];
                    numAction++;
                }
            }

            
            //cout<<"PTM: "<<pillbugTotMoves<<"  TOT:"<<numAction<<endl;
            /*for(int h=0;h<pillbugTotMoves;h++){
                cout<<((pillbugMoves[h])&((((actionT)1)<<63)-1))<<endl;
            }
            for(int k=0;k<numAction;k++){
                cout<<(resAction[k])<<endl;
            }
            bool flag;*/
            /*
            for(int i=0;i<pillbugTotMoves;i++){
                flag=true;
                for(int j=0;j<numAction;j++){
                    if(pillbugMoves[i]==resAction[j]){
                        flag=false;
                        //G.isValidMoveBitmask[G._isValidMoveBitmask_rel_pos[j]]=0;
                        break;
                    }
                }
                if(flag){
                    resAction[numAction]=pillbugMoves[i];
                    numAction++;
                }
            }
        }  */ 
    }
    if(numAction==0){
        //G.isValidMoveBitmask[0]=1;
        //G.associatedAction[0]=0;
        numAction=1;
        resAction[0]=0;
    }
    return;
}


void Board::possibleMovesBug(pieceT b){
    if (b == INVALID_PIECE) return;
    if(col(b)==currentColor()){ // turn is required to make the program efficent
        switch(kind(b)){
            case BEETLE:
                possibleMoves_Beetle(b);
                break;
            case QUEEN:
                possibleMoves_Queen(b);
                break;
            case GRASSHOPPER:
                possibleMoves_Grasshopper(b);
                break;
            case SOLDIER_ANT:
                possibleMoves_SoldierAnt(b);
                break;
            case SPIDER:
                possibleMoves_Spider(b);
                break;
            case MOSQUITO:
                possibleMoves_Mosquito(b);
                break;
            case LADYBUG:
                possibleMoves_Ladybug(b);
                break;
            case PILLBUG:
                possibleMoves_Pillbug(b);
                break;
        }
    }
}



void Board::possibleMoves_Queen(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        positionT pos = G.getPosition(bug);
        for(int i=0;i<6;i++){
            if(G.isFree(applayMove(pos,i))&&G.canSlideToFreeDir(pos,applayMove(pos,i),i)){
                    //G.isValidMoveBitmask[687+i]=1;
                    //G._isValidMoveBitmask_rel_pos[numAction]=687+i;
                    
                    //G.associatedAction[687+i]=
                    resAction[numAction]=movement(bug,applayMove(pos,i));
                    numAction++;
            }
        }
    }
}


void Board::possibleMoves_Beetle(pieceT bug){
    if (G.canPieceMove(bug, currentTurn)){
        positionT pos = G.getPosition(bug);
        int h=G.getHight(pos)-1;
        for(int i=0;i<6;i++){
            
            positionT dest=applayMove(pos,i);
            positionT p1=  applayMove(pos,(i+1)%6);
            positionT p2=  applayMove(pos,(i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    //G.isValidMoveBitmask[693+i+6*(bug&1)]=1;
                    //G._isValidMoveBitmask_rel_pos[numAction]=693+i+6*(bug&1);

                    //G.associatedAction[693+i+6*(bug&1)]=
                    resAction[numAction]=(movement(bug, dest));
                    numAction++;
                }
            }
        }
    }
}

void Board::possibleMoves_Grasshopper(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        positionT from=G.getPosition(bug);
        for(direction dir=0;dir<6;dir++){
            positionT next = applayMove(from,dir);
            if(!G.isFree(next)){
                do{
                    next=applayMove(next,dir);
                } while(!G.isFree(next));
                //G.isValidMoveBitmask[705+dir+6*((bug-5)%14)]=1;
                //G._isValidMoveBitmask_rel_pos[numAction]=705+dir+6*((bug-5)%14);
                //G.associatedAction[705+dir+6*((bug-5)%14)]=
                resAction[numAction]=(movement(bug, next));
                numAction++;
            }
        }
    }
}

void rotBS(bitset<1024> &b, uint64_t l){
    b=(b<<l)| (b>>(1024ull-l));
}

void Board::possibleMoves_SoldierAnt(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        int fQ=0;
        int bQ=0;
        inQueue.reset();
        inQueue.updateOr(G.occupied);

        const positionT startPos=G.getPosition(bug);

        inQueue.set(startPos ,1);
        G.removePiece(bug);
        BoardBitSet isOccupiedNear[6];
        

        for(int i=0;i<6;i++){
            isOccupiedNear[i]=G.occupied;
            isOccupiedNear[i].getRot(i);
        }
        
        BoardBitSet canGoTo[6];
        for(int i=0;i<6;i++){
            canGoTo[i]=(isOccupiedNear[(i+5)%6] ^ isOccupiedNear[(i+1)%6]) & (~isOccupiedNear[i]);
        }

        BoardBitSet reachable;
        reachable.set(startPos,1);
        BoardBitSet pr;
        for(int i=0;i<64;i++){
            pr=reachable;
            for(int j=0;j<6;j++){
                reachable.updateOr((canGoTo[j] & reachable).getRot((j+3)%6));     
            }
            if(reachable==pr)break;
        }
        
        reachable.set(startPos,0);

        for(uint64_t i=0;i<16;i++){
            if(reachable.bv[i]>0){
                for(uint64_t j=0;j<64;j++){
                    if(reachable.bv[i]&(1ull<<(63-j))){
                        resAction[numAction++]=(actionT)(bug|(i<<11)|(j<<5));
                    }
                }
            }
        }G.addPiece(startPos,bug);    
    }
}


void Board::possibleMoves_Spider(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        positionT startPos=G.getPosition(bug);
        bitset<SIZE_BOARD*SIZE_BOARD> reached;
        reached.reset();
        G.removePiece(bug);
        

        
        for(int dr1=0;dr1<6;dr1++){
            positionT next1=applayMove(startPos,dr1);
            positionT p1=applayMove(startPos,(dr1+1)%6);
            positionT p2=applayMove(startPos,(dr1+5)%6);
            if(!G.occupied.get_bit(next1) && (G.occupied.get_bit(p1)^G.occupied.get_bit(p2))){
                for(int dr2=0;dr2<6;dr2++){
                    if(dr1==(dr2+3)%6) continue;
                    positionT next2=applayMove(next1,dr2);
                    positionT p1=applayMove(next1,(dr2+1)%6);
                    positionT p2=applayMove(next1,(dr2+5)%6);
                    if(next2!=startPos && !G.occupied.get_bit(next2) &&   (G.occupied.get_bit(p1)^G.occupied.get_bit(p2))){
                         for(int dr3=0;dr3<6;dr3++){
                            positionT next3=applayMove(next2,dr3);
                            positionT p1=applayMove(next2,(dr3+1)%6);
                            positionT p2=applayMove(next2,(dr3+5)%6);
                            if( !G.occupied.get_bit(next3) && (dr3!=(dr2+3)%6) && (G.occupied.get_bit(p1)^G.occupied.get_bit(p2))){
                                    reached.set(next3,1);
                            }
                         }
       
                    }
                }
            }
        }
        positionT n1,n2,n3,n4,n5,n6;
        for(int dir1=0;dir1<6;dir1++){
            int dir2=(dir1+1)%6;
            n1=applayMove(startPos,dir1);
            n2=      applayMove(n1,dir1);
            n3=      applayMove(n2,dir1);
            n4=      applayMove(n1,dir2);
            n5=      applayMove(n2,dir2);
            n6=      applayMove(n4,dir2);
            positionT rPos[]={n1,n2,n3,n4,n5,n6};
            for(int ni=0;ni<6;ni++){
                if(reached[rPos[ni]]){
                    //G.isValidMoveBitmask[1227+ni+dir1*6+(numInc(bug)-1)*36]=1;
                    //G._isValidMoveBitmask_rel_pos[numAction]=1227+ni+dir1*6+(numInc(bug)-1)*36;
                    //G.associatedAction[1227+ni+dir1*6+(numInc(bug)-1)*36]=
                    resAction[numAction]=movement(bug,rPos[ni]);
                    numAction++;
                }
            }        
        
        }

        G.addPiece(startPos,bug);
    }
}

void Board::possibleMoves_Pillbug(pieceT bug){
    pillbugTotMoves=0;
    positionT pos = G.getPosition(bug);
    
    // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
    
    if (G.canPieceMove(bug,currentTurn)) {
        for(int i=0;i<6;i++){
            if(G.isFree(applayMove(pos,i))&&G.canSlideToFreeDir(pos,applayMove(pos,i),i)){
                //G.isValidMoveBitmask[1299+i]=1;
                //G._isValidMoveBitmask_rel_pos[numAction]=1299+i;
                //G.associatedAction[1299+i]=
                resAction[numAction]=movement(bug,applayMove(pos,i));
                numAction++;
            }
        }
    }
}




void Board::possibleMoves_Ladybug(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        
        positionT startPos=G.getPosition(bug);
        int startPosIDX=startPos ;
        bitset<SIZE_BOARD*SIZE_BOARD> reached;
        reached.reset();
        G.removePiece(bug);
        reached.set(startPosIDX,1);

        
                      
        for(int dr1=0;dr1<6;dr1++){
            positionT next1=applayMove(startPos,dr1);

            int hp1=G.getHight(applayMove(startPos,(dr1+1)%6));
            int hp2=G.getHight(applayMove(startPos,(dr1+5)%6));
            int hPos1=G.getHight(next1);
            int next1IDX=next1 ;
            if(G.occupied.get_bit(next1IDX)&& (hp1<=hPos1 ||hp2<=hPos1) ){
                for(int dr2=0;dr2<6;dr2++){
                    positionT next2=applayMove(next1,dr2);
                    hp1=G.getHight(applayMove(next1,(dr2+1)%6));
                    hp2=G.getHight(applayMove(next1,(dr2+5)%6));
                    int hPos2=G.getHight(next2);
            
                    int next2IDX=next2 ;
                    if(G.occupied.get_bit(next2IDX) && (hp1<=hPos1 ||hp2<=hPos1 || hp1<=hPos2 || hp2<=hPos2)){
                         for(int dr3=0;dr3<6;dr3++){
                            positionT next3=applayMove(next2,dr3);
                            hp1=G.getHight( applayMove(next2,(dr3+1)%6));
                            hp2=G.getHight( applayMove(next2,(dr3+5)%6));
            
                            int next3IDX=next3 ;
                            if(!G.occupied.get_bit(next3IDX) && !reached[next3IDX] && (hp1<=hPos2 || hp2<=hPos2)){
                                    reached.set(next3IDX,1);
                            }
                         }
       
                    }
                }
            }
        }
        G.addPiece(startPos,bug);

         positionT n1,n2,n3,n4,n5,n6;
        for(int dir1=0;dir1<6;dir1++){
            int dir2=(dir1+1)%6;


            n1=applayMove(startPos,dir1);
            n2=      applayMove(n1,dir1);
            n3=      applayMove(n2,dir1);
            n4=      applayMove(n1,dir2);
            n5=      applayMove(n2,dir2);
            n6=      applayMove(n4,dir2);
             positionT nei[]={n1,n2,n3,n4,n5,n6};


            for(int i=0;i<6;i++){
                if(reached[nei[i] ]){
                    //G.isValidMoveBitmask[1227+i+dir1*6+(numInc(bug)-1)*36]=1;
                    //G._isValidMoveBitmask_rel_pos[numAction]=1227+i+dir1*6+(numInc(bug)-1)*36;
                    //G.associatedAction[1227+i+dir1*6+(numInc(bug)-1)*36]= 
                    resAction[numAction]=movement(bug,nei[i]);
                    numAction++;
                }
            }
                        
            
        }
    }
}



void Board::possibleMoves_Mosquito(pieceT bug){  // TODO
    if (!G.canPieceMove(bug, currentTurn)) 
        return;
     positionT pos=G.getPosition(bug);

    if(G.topPiece(pos)!=bug)return;

    if (G.getHight(pos)!=1){
        int h=G.getHight(pos)-1;
        for(int i=0;i<6;i++){
            
             positionT dest=applayMove(pos,i);
             positionT p1=  applayMove(pos,(i+1)%6);
             positionT p2=  applayMove(pos,(i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    //G.isValidMoveBitmask[1539+i]=1;
                    //G._isValidMoveBitmask_rel_pos[numAction]=1539+i;
                    //G.associatedAction[1539+i]=
                    resAction[numAction]=(movement(bug, dest));
                    numAction++;
                }
            }
        }
        return;
    }

    bitset<8> copied;
    for(int dir=0;dir<6;dir++){
         positionT n=applayMove(pos,dir);
        if(!G.isFree(n)){
            copied.set(kind(G.topPiece(n)),1);
        }
    }

    BoardBitSet reachable;
    reachable.reset();
    reachable.set(pos ,1);
    if(copied[kind(1)]){  // SPIDER:
        int startPosIDX=pos ;
        G.removePiece(bug);
        
        for(int dr1=0;dr1<6;dr1++){
            positionT next1=applayMove(pos,dr1);
            int next1IDX=next1 ;
            if(!G.occupied.get_bit(next1IDX) && G.canSlideToFreeDir(pos,next1,dr1)){
                for(int dr2=0;dr2<6;dr2++){
                    positionT next2=applayMove(next1,dr2);
                    int next2IDX=next2 ;
                    if(next2IDX!=startPosIDX && next2IDX!=next1IDX  &&!G.occupied.get_bit(next2IDX) && G.canSlideToFreeDir(next1,next2,dr2)){
                         for(int dr3=0;dr3<6;dr3++){
                            positionT next3=applayMove(next2,dr3);
                            int next3IDX=next3 ;
                            if( !G.occupied.get_bit(next3IDX) &&  
                                next3IDX!=next1IDX  && next3IDX!=next2IDX && next3IDX!=startPosIDX 
                                && !reachable.get_bit(next3IDX) &&G.canSlideToFreeDir(next2,next3,dr3)){
                                    reachable.set(next3IDX,1);
                                    // DONE ON LINES +20
                            }
                         }
       
                    }
                }
            }
        }
        G.addPiece(pos,bug);
    }
    int startPosIDX=pos ;

    if(copied[kind(13)]){  //ladybug
            G.removePiece(bug);                 
            for(int dr1=0;dr1<6;dr1++){
                positionT next1=applayMove(pos,dr1);
                int hp1=G.getHight(applayMove(pos,(dr1+1)%6));
                int hp2=G.getHight(applayMove(pos,(dr1+5)%6));
                int hPos1=G.getHight(next1);
                int next1IDX=next1 ;
                if(G.occupied.get_bit(next1IDX) && (hp1<=hPos1 ||hp2<=hPos1)){
                    for(int dr2=0;dr2<6;dr2++){
                        positionT next2=applayMove(next1,dr2);
                        hp1=G.getHight( applayMove(next1,(dr2+1)%6));
                        hp2=G.getHight( applayMove(next1,(dr2+5)%6));
                        int hPos2=G.getHight(next2);
                        int next2IDX=next2 ;
                        if(G.occupied.get_bit(next2IDX)  && (hp1<=hPos1 ||hp2<=hPos1 || hp1<=hPos2 || hp2<=hPos2)){
                            for(int dr3=0;dr3<6;dr3++){
                                positionT next3=applayMove(next2,dr3);
                                hp1=G.getHight( applayMove(next2,(dr3+1)%6));
                                hp2=G.getHight( applayMove(next2,(dr3+5)%6));
                                int next3IDX=next3 ;
                                if(!G.occupied.get_bit(next3IDX) && !reachable.get_bit(next3IDX)  && (hp1<=hPos2 || hp2<=hPos2)){
                                        reachable.set(next3IDX,1);
                                }
                            }
        
                        }
                    }
                }
            }
            G.addPiece(pos,bug);       
    }
   
    if(copied[kind(3)]){   // beetle
        int h=G.getHight(pos)-1;
        for(int i=0;i<6;i++){
            
            positionT dest=applayMove(pos,i);
            positionT p1=applayMove(pos,(i+1)%6);
            positionT p2=applayMove(pos,(i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    if(!G.isFree(dest)){
                        //G.isValidMoveBitmask[1539+i]=1;
                        //G._isValidMoveBitmask_rel_pos[numAction]=1539+i;
                        //G.associatedAction[1539+i]=
                        resAction[numAction]=(movement(bug, dest));
                        numAction++;      
                    }else{
                        reachable.set(dest ,1);
                    }
                }
            }
        }
    }


    if(copied[kind(5)]){   // grasshopper
        for(direction dir=0;dir<6;dir++){
            positionT next = applayMove(pos,dir);
            if(!G.isFree(next)){
                do{
                    next=applayMove(next,dir);
                } while(!G.isFree(next));
                reachable.set(next ,1);
            }
        }
    } 

    if(copied[kind(8)]){   // queen
        for(int i=0;i<6;i++){
            if(G.isFree(applayMove(pos,i))&&G.canSlideToFreeDir(pos,applayMove(pos,i),i)){
                    reachable.set(applayMove(pos,i) ,1);
            }
        }
    }
    
    if(copied[kind(9)]){    //soldier ant

        inQueue.reset();
        
        const positionT startPos=G.getPosition(bug);

        inQueue.set(startPos ,1);
        G.removePiece(bug);
        BoardBitSet isOccupiedNear[6];
        

        for(int i=0;i<6;i++){
            isOccupiedNear[i]=G.occupied;
            isOccupiedNear[i].getRot(i);
        }
        
        BoardBitSet canGoTo[6];
        for(int i=0;i<6;i++){
            canGoTo[i]=(isOccupiedNear[(i+5)%6] ^ isOccupiedNear[(i+1)%6]) & (~isOccupiedNear[i]);
        }

        BoardBitSet reachableAnt;
        reachableAnt.set(startPos,1);
        BoardBitSet pr;
        for(int i=0;i<54;i++){
            pr=reachableAnt;
            for(int j=0;j<6;j++){
                reachableAnt.updateOr((canGoTo[j] & reachableAnt).getRot((j+3)%6));     
            }
            if(reachableAnt==pr)break;
        }
        
        reachableAnt.set(startPos,0);
        reachable.updateOr(reachableAnt);
        G.addPiece(startPos,bug);     
    }


    if(copied[kind(14)]){  // pillbug
        for(int i=0;i<6;i++){  //same as queen
            if(G.isFree(applayMove(pos,i))&&G.canSlideToFreeDir(pos,applayMove(pos,i),i)){
                reachable.set(applayMove(pos,i) ,1);
            }
        }
    }

    positionT near;
    reachable.set(startPosIDX,0);
    for(uint64_t i=0;i<16;i++){
            if(reachable.bv[i]>0){
                for(uint64_t j=0;j<64;j++){
                    if(reachable.bv[i]&(1ull<<(63-j))){
                        resAction[numAction++]=(actionT)(bug|(i<<11)|(j<<5));
                    }
                }
            }
        }
        
}


void Board::computePillbugMovinPieces(){
    pillbugTotMoves=0;
    for(pieceT bug:{14,12}){   // 14 ->P, 12-> M
        if(col(bug)!=currentColor()) bug+=14;
        if(!G.isPlaced[bug]) continue;
        if(prevMoved[currentColor()]==bug)continue;
        positionT pos=G.getPosition(bug);
        if(bug!=G.topPiece(pos))continue;
        if(G.getHight(pos)!=1)continue;
        // Or it makes other adjacent pieces (even of the opponent) move
        bool canGetFrom[]={false,false,false,false,false,false};
        bool canGoTo[]={false,false,false,false,false,false};
        positionT posS[6];
        pieceT bugN[6];
        pieceT posH[6]={0,0,0,0,0,0};
        bitset<6> isPillbug(0);
        isPillbug.reset();

        for(int dir=0;dir<6;dir++){
            posS[dir]=applayMove(pos,dir);
            posH[dir]=G.getHight(posS[dir]);
            if(posH[dir]>0){

                bugN[dir]=G.topPiece(posS[dir]);
                if(kind(bugN[dir])==BugType::PILLBUG) isPillbug.set(dir,1);
            }
        }

        if(kind(bug)!=BugType::PILLBUG){
            if(!isPillbug.any())
                continue;
        }
        // find all the bugs that i can move
        for(int dir=0;dir<6;dir++){
            
            positionT N=posS[dir];
            int i1=(dir+1)%6;
            int i2=(dir+5)%6;
            if(posH[i1]>=2 && posH[i2]>=2) continue;
            if(G.isFree(N)){
                canGoTo[dir]=true;
            }else if(posH[dir]==1){
                if (G.canMoveWithoutBreakingHiveRule(bugN[dir], currentTurn) && bugN[dir]!=prevMoved[currentColor()] ){
                    canGetFrom[dir]=true;
                }
            }
        }

        int k=0;
        for(int i=0;i<6;i++){
            for(int j=0;j<6;j++){
                if(i==j) continue;
                if(canGetFrom[i] && canGoTo[j]){
                    pillbugMoves[pillbugTotMoves]=movement(G.topPiece(posS[i]),posS[j]);
                    pillbugTotMoves++;
                    //G.isValidMoveBitmask[1545+k]=1;
                    //G.associatedAction[1545+k]=pillbugMoves[pillbugTotMoves-1];
                }
                k++;
            }
        }
    }
}


int Board::isPinned(pieceT bug) {
    return !G.canPieceMove(bug, currentTurn);
}

int Board::isCovered(pieceT bug) {
    return !G.isTop(bug);
}
int Board::friendlyNeighbour(pieceT bug) {
    int res = 0;
    for (int di=0;di<6;di++){
        positionT adj=applayMove(G.getPosition(bug),di);
        if (G.isFree(adj) || col(G.topPiece(adj)) == col(bug)) {
            res++;
        }
    }
    return res;
}
int Board::enemyNeighbour(pieceT bug) {
    int res = 0;
    for (int di=0;di<6;di++){
        positionT adj=applayMove(G.getPosition(bug),di);
        if (!G.isFree(adj) && col(G.topPiece(adj)) != col(bug)) {
            res++;
        }
    }
    return res;
}


string Board::toString() {
    return "Base+MLP;"+GameStateToString(getGameState())+";"+ColorToCompleteString(currentColor())+"["+to_string(currentPlayerTurn())+"]";
}

#endif