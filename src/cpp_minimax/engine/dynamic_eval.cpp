#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"
#include "enums.h"

double DynEval::actionsScore(const Board & b)const {
    // Per ogni bug, valutiamo le azioni che può fare
    pieceT myQueen=(b.currentColor()==PlayerColor::WHITE)?8:22;
    positionT oppositeQueenPosition=b.G.getPosition(30-myQueen);
    double score=0;
    double numQueenMove=0;
    double actBug[32] = {0.0};
    for(int i=0;i<32;i++){
        actBug[i]=0;
    }
    for(int i=0;i<b.numAction;i++){
        actionT action=b.resAction[i];
        pieceT bug=action%32;
        positionT destPos=action>>5;
        actBug[bug]++;

        if (isAdjacent(destPos, oppositeQueenPosition)) {
            score += W.noisyWeight(kind(bug));
        } else {
            score += W.quietWeight(kind(bug));
        }
    }

    for(int i=0;i<32;i++){
        if(actBug[i] == 0){
            score+=W.isPinnedWeight(kind(i));
        }
    }

    //clog << "actionsScore: " << score << endl;
    return score;
}
#include <immintrin.h>


double DynEval::positionalScore(const Board &b)const {
    // Valutiamo la posizione del bug, se è coperto, vicinato..
    double score=0;

    PlayerColor currentColor=b.currentColor();
    PlayerColor oppositeColor=(currentColor==PlayerColor::WHITE?PlayerColor::BLACK:PlayerColor::WHITE);

    alignas(64) uint8_t nS[1024] = {0};
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
    double bugScore[32] = {0.0};


    for(int p=1;p<=28;p++){
        bugScore[p]=0;
        if(b.G.isPlaced[p]){
            bugScore[p]=W.placedWeight(kind(p));
            positionT currPos=b.G.getPosition(p);

            if(b.G.topPiece(currPos)!=p){
                bugScore[p]+=W.isCoveredWeight(kind(p));
            }
            
            bugScore[p]+=((double)(nS[currPos]>>3))*W.enemyCloseWeight(kind(p))+
                    ((double)((nS[currPos]>>3)+(nS[currPos]&7)))*W.friendCloseWeight(kind(p));
        }
    }
    
    for(int i=baseMy;i<=baseMy+13;i++)score+=bugScore[i];
    for(int i=baseOpp;i<=baseOpp+13;i++)score-=bugScore[i];
    
    //clog << "positionalScore: " << score << endl;
    return score;
}

double DynEval::strategicScore(const Board &b) const {
    double score = 0;
    PlayerColor color = b.currentColor();
    int base = (color == PlayerColor::WHITE) ? 1 : 15;
    positionT queenPos = b.G.getPosition(base + 7); // Queen index is 8 or 22

    for (int i = base; i <= base + 13; i++) {
        if (!b.G.isPlaced[i]) continue;
        pieceT p = i;
        positionT pPos = b.G.getPosition(p);
        int k = kind(p);

        if (k == PILLBUG) {
            if (isAdjacent(pPos, queenPos)) {
                score += W.pillbugNearQueenBonus();
            }
        }
        else if (k == MOSQUITO) {
            for (int d = 0; d < 6; d++) {
                positionT neighbor = (pPos + dirDif[d]) & 1023;
                if (b.G.occupied.get_bit(neighbor)) {
                    pieceT top = b.G.topPiece(neighbor);
                    if (kind(top) == SOLDIER_ANT || kind(top) == BEETLE) {
                        score += W.mosquitoNearStrongBugBonus();
                    }
                }
            }
        }
        else if (k == SPIDER) {
            int dist = distanceToQueen(pPos, queenPos);
            score -= dist * W.spiderDistancePenalty();
        }
    }

    //clog << "strategicScore: " << score << endl;

    return score;
}


double DynEval::topologyScore(const Board &b) const {
    double score = 0;
    PlayerColor color = b.currentColor();
    int myQueen = (color == PlayerColor::WHITE) ? 8 : 22;
    positionT queenPos = b.G.getPosition(myQueen);

    int surrounding = isQueenAlmostSurrounded(b, queenPos);
    score -= pow(10.0, surrounding);

    if (isGateFormationAroundQueen(b, queenPos)) {
        score += W.queenGateDefenseBonus();
    }
    if (detectRingFormation(b)) {
        score -= W.ringPenalty();
    }

    //clog << "topologyScore: " << score << endl;
    return score;
}



double DynEval::evalBoardCurrentPlayer(Board & b) const{

    //clog << "calculating board evaluation" << endl;

    if(b.getGameState()>1){// implement suicide
        if (b.getGameState()==BLACK_WIN && b.currentColor()==PlayerColor::BLACK || b.getGameState()==WHITE_WIN && b.currentColor()==PlayerColor::WHITE){
            return 1e10;
        } 
        if (b.getGameState() == DRAW) return 0;
        return -1e10; //check if I won or the othe
    }
    b.ComputePossibleMoves();

    double score=0;
    score += actionsScore(b);  // for each action, compute the utility of the action
    score += positionalScore(b);  // for each piece, compute the utility based on neighbor
    score += strategicScore(b);   // for some piece, if they are close to the queen can be helpful
    score += topologyScore(b);   // find if there are rings or other 
    
    // TODO: qualcosa di non lineare

    return score;
}