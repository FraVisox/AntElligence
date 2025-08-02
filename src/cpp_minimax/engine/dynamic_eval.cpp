#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"



double DynEval::actionsScore(const Board & b)const {
    return 0;
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
    for(int p=1;p<=28;p++){
        double bugScore=0;
        if(b.G.isPlaced[p]){
            bugScore+=W.placedWeight(kind(p));
            if(!b.G.isTop(p)){
                bugScore+=W.converedWeight(kind(p));
            }
            positionT currPos=b.G.getPosition(p);
            int numEnemy=0;
            int numTot=0;
            for(int i=0;i<6;i++){
                positionT nearPos=applayMove(currPos,i);
                if(b.G.occupied.get_bit(nearPos)){
                    numTot++;
                    if(col(b.G.topPiece(nearPos))==oppositeColor)
                        numEnemy++;
                }
            }
            bugScore+=((double)numEnemy)*W.numEnemyCloseWeight(kind(p))+
                    ((double)numTot)*W.totalNumCloseWeight(kind(p));
        }
        if(col(p)==currentColor){
            score+=bugScore;
        }else
            score-=bugScore;
    }
    return score;
}

double DynEval::strategicScore(const Board &b)const {
    return 0;
}

double DynEval::topologyScore(const Board &b)const {
    return 0;   
}


double DynEval::evalBoardCurrentPlayer(Board & b){
    if(b.getGameState()>1){
        return -1e7;
    }


    currentColor=b.currentColor();
    oppositeColor=currentColor==PlayerColor::BLACK?PlayerColor::WHITE:PlayerColor::BLACK;
    myQueen=(currentColor==PlayerColor::WHITE)?8:22;
    oppositeQueen=(currentColor==PlayerColor::WHITE)?22:8;
    myQueenPosition=oppositeQueenPosition=-1;
    if(b.G.isPlaced[myQueen])
        myQueenPosition=b.G.getPosition(myQueen);
    if(b.G.isPlaced[oppositeQueen])
        oppositeQueenPosition=b.G.getPosition(oppositeQueen);

    double score=0;
    score += actionsScore(b);  // for each action, compute the utility of the action
    score += positionalScore(b);  // for each piece, compute the utility based on neighbor
    score += 0;//strategicScore(b);   // for some piece, if they are close to the queen can be helpful
    score += 0;//topologyScore(b);   // find if there are rings or other 
    return score;
}