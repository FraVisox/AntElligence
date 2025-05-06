#include "board.h"
#include "../graph_board/update_board.h"
#include <regex>


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
    state = STARTED;
    //We do not change the type of game
    inHandPiece.reset();
    prevMoved[0]=0;
    prevMoved[1]=0;
};



/**
 * \brief Copy the given board into this one.
 *
 * Copy the given board into this one, overwriting all the members of this
 * board with the corresponding members of the given board.
 *
 * \param b The board to copy from.
 */
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



int Board::countSurrounding(pieceT p) {
    position pos = G.getPosition(p);
    int ret = 0;
    for (direction dir=0;dir<6;dir++){
        position adj= pos.applayMove(dir);
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
    this->state=b.state;
    this->type=b.type;
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

void Board::applayAction(actionT a){
    UpdateBoardE(*this,a);

}


/**
 * 
 * \brief Get all possible moves for the current player.
 *
 * This function returns all possible moves for the current player.
 * It returns a vector of action objects, each representing a possible
 * move. The moves are determined by the current state of the game and
 * the pieces on the board.
 *
 * The function first checks if the board is empty, and if it is, it
 * returns an empty vector. Then it checks if the current player has
 * already placed their queen, and if they have, it returns a vector
 * containing all possible moves for the current player. If the current
 * player has not placed their queen, it returns a vector containing
 * all possible moves for the current player, including placing the
 * queen.
 *
 * If the current player has placed all their pieces, the function
 * returns a vector containing all possible moves for the current
 * player, including passing.
 *
 * If the current player has placed some pieces, but not all of them,
 * the function returns a vector containing all possible moves for
 * the current player, including placing pieces and passing.
 *
 * If the current player has not placed any pieces, the function
 * returns a vector containing all possible moves for the current
 * player, including placing pieces and passing.
 *
 * \return A vector of action objects, each representing a possible
 *         move for the current player.
 */


void Board::ComputePossibleMoves(){
    numAction=0;    
    G.isValidMoveBitmask.reset();
    // 1 If turn == 1, then place something that is not the queen
    if(currentTurn==1){
        for(int b=1;b<=28;b++){
            if(inHandPiece[b]){
            if(col(b)==currentColor() && kind(b) != QUEEN && (numInc(b) == 0 || numInc(b) == 1)) {
                G.isValidMoveBitmask.set(b,1);
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
                    resAction[numAction]=placePiece(b,position{0,0}.applayMove(dir),G);
                    numAction++;
                }
            }
            }
        }
        return;
    }

    // 3 - If i didn't place the queen after turn 3, it's the only move
    if(!placedQueen() && currentPlayerTurn()>3){
        auto positions = G.validPositionPlaceNew(currentColor());
        for(auto pos: positions){
            // SOLVED IN PLACE PIECE
            resAction[numAction]=placePiece(((currentColor()==PlayerColor::WHITE)?8:22),pos,G);
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
        auto positions=G.validPositionPlaceNew(currentColor());
        for(auto p:inHandCol) {
            if (p == INVALID_PIECE) {
                continue;
            }
            for(auto pos : positions){
                // SOLVED IN PLACE PIECE
                resAction[numAction]=placePiece(p, pos,G);
                numAction++;
            }
        }
    }

    // 5- moves
    if(placedQueen()){
        pillbugTotMoves=0;

        for(pieceT b=1;b<=28;b++){
            if(!G.isPlaced[b])continue;
            if (b == prevMoved[currentColor()]) continue;
            possibleMovesBug(b);
            
            if(pillbugTotMoves>0){
                sort(resAction,resAction+numAction);
                sort(pillbugMoves,pillbugMoves+pillbugTotMoves);
                int lO=numAction;
                int j=0;
                for(int i=0;i<lO && j<pillbugTotMoves;i++){
                    if(resAction[i]==pillbugMoves[j]){
                        j++;
                    }else if(resAction[i]>pillbugMoves[j]){
                        resAction[numAction]=pillbugMoves[j];
                        numAction++;
                        j++;
                    }
                }
            }        
        }
    }
    if(numAction==0){
        G.isValidMoveBitmask.set(0,1);
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
        position pos = G.getPosition(bug);
        for(int i=0;i<6;i++){
            if(G.isFree(pos.applayMove(i))&&G.canSlideToFree(pos,pos.applayMove(i))){
                    G.isValidMoveBitmask.set(687+i,1);
                    resAction[numAction]=movement(bug,pos.applayMove(i),G);
                    numAction++;
            }
        }
    }
}


void Board::possibleMoves_Beetle(pieceT bug){
    if (G.canPieceMove(bug, currentTurn)){
        position pos = G.getPosition(bug);
        int h=G.getHight(pos)-1;
        for(int i=0;i<6;i++){
            
            position dest=pos.applayMove(i);
            position p1=pos.applayMove((i+1)%6);
            position p2=pos.applayMove((i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    G.isValidMoveBitmask.set(693+i+6*(bug&1),1);
                    resAction[numAction]=(movement(bug, dest,G));
                    numAction++;
                }
            }
        }
    }
}

void Board::possibleMoves_Grasshopper(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        position from=G.getPosition(bug);
        for(direction dir=0;dir<6;dir++){
            position next = from.applayMove(dir);
            if(!G.isFree(next)){
                do{
                    next=next.applayMove(dir);
                } while(!G.isFree(next));
                G.isValidMoveBitmask.set(705+dir+6*((bug-5)%14));
                resAction[numAction]=(movement(bug, next,G));
                numAction++;
            }
        }
    }
}


void Board::possibleMoves_SoldierAnt(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {

        int fQ=0;
        int bQ=0;
        inQueue.reset();
        inQueue|=G.occupied;

        const position startPos=G.getPosition(bug);
        quePM[fQ++]=startPos;
        position neighbors[6];
        int hights[6];
        bool frees[6];
        inQueue.set(startPos.toInt(),1);
        G.removePiece(bug);
        

        while(fQ!=bQ){
            
            const position& current=quePM[bQ++];
            
            for(int dir=0;dir<6;++dir){
                neighbors[dir]=current.applayMove(dir);
                hights[dir]=G.getHight(neighbors[dir]);
                frees[dir]=G.isFree(neighbors[dir]);
            }
            int currentHight=G.getHight(current);

            for(int dir=0;dir<6;++dir){
                
                const position& neighbor = neighbors[dir];
                const int neighborIdx = neighbor.toInt();
                if(inQueue[neighborIdx]) 
                    continue;
                const int &p1=(dir+1)%6;
                const int &p2=(dir+5)%6;

                int minH=min(currentHight,hights[dir]);

                bool isGateT=(hights[p1]>minH && hights[p2]>minH);
                bool isDijoined=(frees[p1])&& (frees[p2]);
                if(!isGateT && !isDijoined){
                    pieceT near=(!frees[p1])?G.topPiece(current.applayMove(p1)):G.topPiece(current.applayMove(p2));
                    G.isValidMoveBitmask.set(
                        723+  // base
                        dir+
                        6*(numInc(bug)-1)+
                        6*3*(near-1)
                        ,1);


                    resAction[numAction++]=(movement(bug,neighbor,G));
                    quePM[fQ++]=neighbor;
                    inQueue.set(neighborIdx,1);
                }
            }
        }
        G.addPiece(startPos,bug);
                
    }
}


void Board::possibleMoves_Spider(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        position startPos=G.getPosition(bug);
        int startPosIDX=startPos.toInt();
        bitset<SIZE_BOARD*SIZE_BOARD> reached;
        reached.reset();
        G.removePiece(bug);
        
        for(int dr1=0;dr1<6;dr1++){
            position next1=startPos.applayMove(dr1);
            int next1IDX=next1.toInt();
            if(!G.occupied[next1IDX] && G.canSlideToFreeDir(startPos,next1,dr1)){
                for(int dr2=0;dr2<6;dr2++){
                    position next2=next1.applayMove(dr2);
                    int next2IDX=next2.toInt();
                    if(next2IDX!=startPosIDX && next2IDX!=next1IDX  &&!G.occupied[next2IDX] && G.canSlideToFreeDir(next1,next2,dr2)){
                         for(int dr3=0;dr3<6;dr3++){
                            position next3=next2.applayMove(dr3);
                            int next3IDX=next3.toInt();
                            if( !G.occupied[next3IDX] &&  
                                next3IDX!=next1IDX  && next3IDX!=next2IDX && next3IDX!=startPosIDX 
                                && !reached[next3IDX] &&G.canSlideToFreeDir(next2,next3,dr3)){
                                    reached.set(next3IDX,1);

                                    // DONE ON LINES +10
                                    resAction[numAction]=movement(bug,next3,G);
                                    numAction++;
                            }
                         }
       
                    }
                }
            }
        }
        position n1,n2,n3,n4,n5,n6;
        for(int dir1=0;dir1<6;dir1++){
            int dir2=(dir1+1)%6;
            
            n1=startPos.applayMove(dir1);
            n2=      n1.applayMove(dir1);
            n3=      n2.applayMove(dir1);
            n4=      n1.applayMove(dir2);
            n5=      n2.applayMove(dir2);
            n6=      n4.applayMove(dir2);

            if(reached[n1.toInt()])G.isValidMoveBitmask.set(1227+0+dir1*6+(numInc(bug)-1)*36,1);
            if(reached[n2.toInt()])G.isValidMoveBitmask.set(1227+1+dir1*6+(numInc(bug)-1)*36,1);
            if(reached[n3.toInt()])G.isValidMoveBitmask.set(1227+2+dir1*6+(numInc(bug)-1)*36,1);            
            if(reached[n4.toInt()])G.isValidMoveBitmask.set(1227+3+dir1*6+(numInc(bug)-1)*36,1);
            if(reached[n5.toInt()])G.isValidMoveBitmask.set(1227+4+dir1*6+(numInc(bug)-1)*36,1);
            if(reached[n6.toInt()])G.isValidMoveBitmask.set(1227+5+dir1*6+(numInc(bug)-1)*36,1);            
        
        }

        G.addPiece(startPos,bug);
    }
}

void Board::possibleMoves_Pillbug(pieceT bug){

    position pos = G.getPosition(bug);

    // The pillbug can move as a queen. Check if the move is already present as the pillbug could be moved by the mosquito.
    
    if (G.canPieceMove(bug,currentTurn)) {
        position pos = G.getPosition(bug);
        for(int i=0;i<6;i++){
            if(G.isFree(pos.applayMove(i))&&G.canSlideToFree(pos,pos.applayMove(i))){
                    G.isValidMoveBitmask.set(1299+i,1);
                    resAction[numAction]=movement(bug,pos.applayMove(i),G);
                    numAction++;
            }
        }
    }
    

    // Or it makes other adjacent pieces (even of the opponent) move
    bool canGetFrom[]={false,false,false,false,false,false};
    bool canGoTo[]={false,false,false,false,false,false};
    position posS[6];

    for(int dir=0;dir<6;dir++)posS[dir]=pos.applayMove(dir);
    // find all the bugs that i can move
    for(int dir=0;dir<6;dir++){
        
        position N=posS[dir];
        position p1=posS[(dir+1)%6];
        position p2=posS[(dir+5)%6];
        if (G.getHight(p1)>=2 && G.getHight(p2)>=2) continue;
        if (G.getHight(N)>1) continue;
                
        pieceT possible_piece = G.topPiece(N);
        if (G.canMoveWithoutBreakingHiveRule(possible_piece, currentTurn) && possible_piece!=prevMoved[1-currentColor()]){
            canGetFrom[dir]=true;
        }
        if (G.isFree(N)){
            canGoTo[dir]=true;
        }
    }

    int k=0;
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            if(i==j) continue;
            if(canGetFrom[i] && canGoTo[j]){
                pillbugMoves[pillbugTotMoves]=movement(G.topPiece(posS[i]),posS[j],G);
                pillbugTotMoves++;
                G.isValidMoveBitmask.set(1305+k,i);
            }
            k++;
        }
    }
}


void Board::possibleMoves_Ladybug(pieceT bug){
    if (G.canPieceMove(bug,currentTurn)) {
        
        position startPos=G.getPosition(bug);
        int startPosIDX=startPos.toInt();
        bitset<SIZE_BOARD*SIZE_BOARD> reached;
        reached.reset();
        G.removePiece(bug);
        reached.set(startPosIDX,1);
                                    
        for(int dr1=0;dr1<6;dr1++){
            position next1=startPos.applayMove(dr1);
            int next1IDX=next1.toInt();
            if(G.occupied[next1IDX]){
                for(int dr2=0;dr2<6;dr2++){
                    position next2=next1.applayMove(dr2);
                    int next2IDX=next2.toInt();
                    if(G.occupied[next2IDX] ){
                         for(int dr3=0;dr3<6;dr3++){
                            position next3=next2.applayMove(dr3);
                            int next3IDX=next3.toInt();
                            if(!G.occupied[next3IDX] && !reached[next3IDX]){
                                    reached.set(next3IDX,1);
                                    resAction[numAction]=movement(bug,next3,G);
                                    numAction++;
                            }
                         }
       
                    }
                }
            }
        }
        G.addPiece(startPos,bug);

        position n1,n2,n3,n4,n5,n6;
        for(int dir1=0;dir1<6;dir1++){
            int dir2=(dir1+1)%6;
            
            n1=startPos.applayMove(dir1);
            n2=      n1.applayMove(dir1);
            n3=      n2.applayMove(dir1);
            n4=      n1.applayMove(dir2);
            n5=      n2.applayMove(dir2);
            n6=      n4.applayMove(dir2);

            if(reached[n1.toInt()])G.isValidMoveBitmask.set(1335+0+dir1*6,1);
            if(reached[n2.toInt()])G.isValidMoveBitmask.set(1335+1+dir1*6,1);
            if(reached[n3.toInt()])G.isValidMoveBitmask.set(1335+2+dir1*6,1);            
            if(reached[n4.toInt()])G.isValidMoveBitmask.set(1335+3+dir1*6,1);
            if(reached[n5.toInt()])G.isValidMoveBitmask.set(1335+4+dir1*6,1);
            if(reached[n6.toInt()])G.isValidMoveBitmask.set(1335+5+dir1*6,1);            
        }
    }
}



void Board::possibleMoves_Mosquito(pieceT bug){  // TODO
    if (!G.canPieceMove(bug, currentTurn)) 
        return;
    position pos=G.getPosition(bug);


    if (!G.getHight(pos)!=1){
        int h=G.getHight(pos)-1;
        for(int i=0;i<6;i++){
            
            position dest=pos.applayMove(i);
            position p1=pos.applayMove((i+1)%6);
            position p2=pos.applayMove((i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    resAction[numAction]=(movement(bug, dest,G));
                    numAction++;
                    G.isValidMoveBitmask.set(1539+i,1);
                }
            }
        }
        return;
    }

    bitset<8> copied;
    for(int dir=0;dir<6;dir++){
        position n=pos.applayMove(dir);
        if(!G.isFree(n) && kind(G.topPiece(n))){
            copied.set(kind(G.topPiece(n)),1);
        }
    }

    bitset<32*32> reachable;
    reachable.reset();
    reachable.set(pos.toInt(),1);
    if(copied[kind(1)]){  // SPIDER:
        int startPosIDX=pos.toInt();
        G.removePiece(bug);
        
        for(int dr1=0;dr1<6;dr1++){
            position next1=pos.applayMove(dr1);
            int next1IDX=next1.toInt();
            if(!G.occupied[next1IDX] && G.canSlideToFreeDir(pos,next1,dr1)){
                for(int dr2=0;dr2<6;dr2++){
                    position next2=next1.applayMove(dr2);
                    int next2IDX=next2.toInt();
                    if(next2IDX!=startPosIDX && next2IDX!=next1IDX  &&!G.occupied[next2IDX] && G.canSlideToFreeDir(next1,next2,dr2)){
                         for(int dr3=0;dr3<6;dr3++){
                            position next3=next2.applayMove(dr3);
                            int next3IDX=next3.toInt();
                            if( !G.occupied[next3IDX] &&  
                                next3IDX!=next1IDX  && next3IDX!=next2IDX && next3IDX!=startPosIDX 
                                && !reachable[next3IDX] &&G.canSlideToFreeDir(next2,next3,dr3)){
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
    int startPosIDX=pos.toInt();

    if(copied[kind(13)]){  //ladybug
            G.removePiece(bug);                 
            for(int dr1=0;dr1<6;dr1++){
                position next1=pos.applayMove(dr1);
                int next1IDX=next1.toInt();
                if(G.occupied[next1IDX]){
                    for(int dr2=0;dr2<6;dr2++){
                        position next2=next1.applayMove(dr2);
                        int next2IDX=next2.toInt();
                        if(G.occupied[next2IDX] ){
                            for(int dr3=0;dr3<6;dr3++){
                                position next3=next2.applayMove(dr3);
                                int next3IDX=next3.toInt();
                                if(!G.occupied[next3IDX] && !reachable[next3IDX]){
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
            
            position dest=pos.applayMove(i);
            position p1=pos.applayMove((i+1)%6);
            position p2=pos.applayMove((i+5)%6);

            if(!G.isFree(p1) || !G.isFree(p2) || !G.isFree(dest) || h>0){
                int maxH=max(h,G.getHight(dest));
                
                if(G.getHight(p1)<=maxH || G.getHight(p2)<=maxH){
                    if(!G.isFree(dest)){
                        G.isValidMoveBitmask.set(1539+i,1);
                    }
                    reachable.set(dest.toInt(),1);
                }
            }
        }
    }


    if(copied[kind(5)]){   // grasshopper
        for(direction dir=0;dir<6;dir++){
            position next = pos.applayMove(dir);
            if(!G.isFree(next)){
                do{
                    next=next.applayMove(dir);
                } while(!G.isFree(next));
                reachable.set(next.toInt(),1);
            }
        }
    } 

    if(copied[kind(8)]){   // queen
        for(int i=0;i<6;i++){
            if(G.isFree(pos.applayMove(i))&&G.canSlideToFree(pos,pos.applayMove(i))){
                    reachable.set(pos.applayMove(i).toInt(),1);
            }
        }
    }
    
    if(copied[kind(9)]){    //soldier ant

        int fQ=0;
        int bQ=0;
        inQueue.reset();
        inQueue|=G.occupied;

        const position startPos=G.getPosition(bug);
        quePM[fQ++]=startPos;
        position neighbors[6];
        int hights[6];
        bool frees[6];
        inQueue.set(startPos.toInt(),1);
        G.removePiece(bug);
        

        while(fQ!=bQ){
            
            const position& current=quePM[bQ++];
            
            for(int dir=0;dir<6;++dir){
                neighbors[dir]=current.applayMove(dir);
                hights[dir]=G.getHight(neighbors[dir]);
                frees[dir]=G.isFree(neighbors[dir]);
            }
            int currentHight=G.getHight(current);

            for(int dir=0;dir<6;++dir){
                
                const position& neighbor = neighbors[dir];
                const int neighborIdx = neighbor.toInt();
                if(inQueue[neighborIdx]) 
                    continue;
                const int &p1=(dir+1)%6;
                const int &p2=(dir+5)%6;

                int minH=min(currentHight,hights[dir]);

                bool isGateT=(hights[p1]>minH && hights[p2]>minH);
                bool isDijoined=(frees[p1])&& (frees[p2]);
                if(!isGateT && !isDijoined){
                    reachable.set(neighbor.toInt(),1);
                    quePM[fQ++]=neighbor;
                    inQueue.set(neighborIdx,1);
                }
            }
        }
        G.addPiece(startPos,bug);
    }


    if(copied[kind(14)]){  // pillbug
        for(int i=0;i<6;i++){  //same as queen
            if(G.isFree(pos.applayMove(i))&&G.canSlideToFree(pos,pos.applayMove(i))){
                    reachable.set(pos.applayMove(i).toInt(),1);
            }
        }


        // Or it makes other adjacent pieces (even of the opponent) move
        bool canGetFrom[]={false,false,false,false,false,false};
        bool canGoTo[]={false,false,false,false,false,false};
        position posS[6];

        for(int dir=0;dir<6;dir++)posS[dir]=pos.applayMove(dir);
        // find all the bugs that i can move
        for(int dir=0;dir<6;dir++){
            
            position N=posS[dir];
            position p1=posS[(dir+1)%6];
            position p2=posS[(dir+5)%6];
            if (G.getHight(p1)>=2 && G.getHight(p2)>=2) continue;
            if (G.getHight(N)>1) continue;
                    
            pieceT possible_piece = G.topPiece(N);
            if (G.canMoveWithoutBreakingHiveRule(possible_piece, currentTurn) && possible_piece!=prevMoved[1-currentColor()]){
                canGetFrom[dir]=true;
            }
            if (G.isFree(N)){
                canGoTo[dir]=true;
            }
        }

        int k=0;
        for(int i=0;i<6;i++){
            for(int j=0;j<6;j++){
                if(i==j) continue;
                if(canGetFrom[i] && canGoTo[j]){
                    pillbugMoves[pillbugTotMoves]=movement(G.topPiece(posS[i]),posS[j],G);
                    pillbugTotMoves++;
                    G.isValidMoveBitmask.set(1545+k,1);
                }
                k++;
            }
        }
    }

    position near;
    for(int pi=0;pi<1024;pi++){
        if(reachable[pi] && pi!=startPosIDX){
            position dest(pi);
            resAction[numAction]=(movement(bug, dest,G));
            numAction++;

            for(int dir=0;dir<6;dir++){
                near=dest.applayMove(dir);
                if(!G.isFree(near) && G.topPiece(near)!=bug){
                    pieceT bugNear=G.topPiece(near);
                    G.isValidMoveBitmask.set(
                    1371+  // base
                    dir+
                    6*(bugNear-1)
                    ,1);    
                }                
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
    for (position adj: G.getPosition(bug).neighbor()) {
        if (G.isFree(adj) || col(G.topPiece(adj)) == col(bug)) {
            res++;
        }
    }
    return res;
}
int Board::enemyNeighbour(pieceT bug) {
    int res = 0;
    for (position adj: G.getPosition(bug).neighbor()) {
        if (!G.isFree(adj) && col(G.topPiece(adj)) != col(bug)) {
            res++;
        }
    }
    return res;
}


#endif