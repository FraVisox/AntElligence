#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"



double DynEval::actionsScore(const Board & b)const {
    // Per ogni bug, valutiamo le azioni che può fare
    pieceT myQueen=(b.currentColor()==PlayerColor::WHITE)?8:22;
    positionT oppositeQueenPosition=b.G.getPosition(30-myQueen);
    double score=0;
    double numQueenMove=0;
    double actBug[32];
    for(int i=0;i<32;i++){
        actBug[i]=0;
    }
    for(int i=0;i<b.numAction;i++){
        actionT action=b.resAction[i];
        pieceT bug=action%32;
        positionT destPos=action>>5;
        actBug[bug]++;

        if(b.G.occupied.get_bit(destPos)){
            //...
        }
        // if destPost == any(near queen)

    }
    
    for(int i=1;i<=28;i++){
        score+=(double)actBug[i]*W.numActionWeight(kind(i));
    }
    return score;
}
#include <immintrin.h>


double DynEval::positionalScore(const Board &b)const {
    // Valutiamo la posizione del bug, se è coperto, vicinato..
    double score=0;

    PlayerColor currentColor=b.currentColor();
    PlayerColor oppositeColor=(currentColor==PlayerColor::WHITE?PlayerColor::BLACK:PlayerColor::WHITE);

    alignas(64) uint8_t nS[1024];
    for(int i=0;i<1024;i++)
    {
        nS[i]=0;
    }

    int baseMy=(currentColor==PlayerColor::WHITE?1:15);
    int baseOpp=16-baseMy;

    for(int i=baseMy;i<=baseMy+13;i++){
        if(b.G.isPlaced[i]){
            positionT myPos=b.G.bugPosition[i];
            for(int d=0;d<6;d++){
                nS[(myPos+dirDif[d])&1023]+=1;
            }
        }
    }
    for(int i=baseOpp;i<=baseOpp+13;i++){
        if(b.G.isPlaced[i]){
            positionT myPos=b.G.bugPosition[i];
            for(int d=0;d<6;d++){
                nS[(myPos+dirDif[d])&1023]+=8;
            }
        }
    }
    double bugScore[32];


    for(int p=1;p<=28;p++){
        bugScore[p]=0;
        if(b.G.isPlaced[p]){
            bugScore[p]=W.placedWeight(kind(p));
            positionT currPos=b.G.getPosition(p);

            if(b.G.topPiece(currPos)!=p){
                bugScore[p]+=W.converedWeight(kind(p));
            }
            
            bugScore[p]+=((double)(nS[currPos]>>3))*W.numEnemyCloseWeight(kind(p))+
                    ((double)((nS[currPos]>>3)+(nS[currPos]&7)))*W.totalNumCloseWeight(kind(p));
        }
    }
    
    for(int i=baseMy;i<=baseMy+13;i++)score+=bugScore[i];
    for(int i=baseOpp;i<=baseOpp+13;i++)score-=bugScore[i];
    
    return score;
}

double DynEval::strategicScore(const Board &b)const {
    // Tipo pillbug affianco alla queen, la mosquito vicino a pezzi buoni, così via danno punti
    return 0;
}

double DynEval::topologyScore(const Board &b)const {
    // Se c'è un anello non ci piace, se la queen è coperta da gate ci piace...
    return 0;   
}


double DynEval::evalBoardCurrentPlayer(Board & b) const{

    if(b.getGameState()>1){// implement suicide
        return -1e10; //check if I won or the othe
    }
    b.ComputePossibleMoves();
    //b.ComputePossibleMoves();

    double score=0;
    score += actionsScore(b);  // for each action, compute the utility of the action
    score += positionalScore(b);  // for each piece, compute the utility based on neighbor
    score += strategicScore(b);   // for some piece, if they are close to the queen can be helpful
    score += topologyScore(b);   // find if there are rings or other 
    
    // TODO: qualcosa di non lineare

    return score;
}