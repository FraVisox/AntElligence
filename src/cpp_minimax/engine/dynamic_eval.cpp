#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"



double DynEval::actionsScore(const Board & b)const {
    pieceT myQueen=(b.currentColor()==PlayerColor::WHITE)?8:22;
    positionT oppositeQueenPosition=b.G.getPosition(30-myQueen);
    double scoreNoisyMoves=0;
    double numQueenMove=0;
    for(int i=0;i<b.numAction;i++){
        actionT action=b.resAction[i];
        pieceT bug=action%32;
        positionT destPos=action>>5;
        for(int d=0;d<6;d++){
            positionT resPos=applayMove(destPos,d);
            if(resPos==oppositeQueenPosition)
                scoreNoisyMoves+=W.noisyWeight(kind(bug));
        }
        if(bug==myQueen){
            numQueenMove+=1;
        }
    }
    return W.QueenMoveWeight()*numQueenMove+
            scoreNoisyMoves+
            b.numAction*W.numActionWeight();
}

double DynEval::positionalScore(const Board &b)const {
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
    return 0;
}

double DynEval::topologyScore(const Board &b)const {
    return 0;   
}


double DynEval::evalBoardCurrentPlayer(Board & b) const{
    if(b.getGameState()>1){
        return -1e7;
    }
    //b.ComputePossibleMoves();

    

    double score=0;
    score += //actionsScore(b);  // for each action, compute the utility of the action
    score += positionalScore(b);  // for each piece, compute the utility based on neighbor
    score += 0;//strategicScore(b);   // for some piece, if they are close to the queen can be helpful
    score += 0;//topologyScore(b);   // find if there are rings or other 
    return score;
}