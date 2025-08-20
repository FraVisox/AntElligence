#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"
#include "enums.h"
#include <chrono>

// =====================================================
// Utility functions
// =====================================================

constexpr inline bool isAdjacent(positionT a, positionT b) {
    for (int i = 0; i < 6; i++) {
        if (((a + dirDif[i]) & 1023) == (b & 1023)) return true;
    }
    return false;
}

constexpr inline int distanceToQueen(positionT a, positionT b) {
    // Approximate distance on hex grid (valid since it's small)
    int dx = ((a & 31) - (b & 31));
    int dy = ((a >> 5) - (b >> 5));
    return abs(dx) + abs(dy);
}

bool isGateFormationAroundQueen(const Board& b, positionT qPos) {
    int gateCount = 0;
    for (int i = 0; i < 6; i++) {
        positionT p1 = (qPos + dirDif[i]) & 1023;
        positionT p2 = (qPos + dirDif[(i+1)%6]) & 1023;
        if (b.G.occupied.get_bit(p1) && b.G.occupied.get_bit(p2)) {
            // Check if the space between them is not accessible (tight gap)
            positionT target = (qPos + dirDif[i] + dirDif[(i+1)%6]) / 2; // approx
            if (!b.G.occupied.get_bit(target)) {
                gateCount++;
            }
        }
    }
    return gateCount >= 2; // 2 gates or more is generally defensive
}

constexpr inline int isQueenAlmostSurrounded(const Board &b, positionT qPos) {
    int count = 0;
    for (int i = 0; i < 6; i++) {
        positionT n = (qPos + dirDif[i]) & 1023;
         count += b.G.occupied.get_bit(n);
    }
    return count;
}

static bool detectRingFormation(const Board &b) {      // if all the position are free next to it

    BoardBitSet isValid;
    isValid=(~b.G.occupied);

    for(int d=0;d<6;d++){
        isValid.updateAnd(b.G.occupied.getRot(d));
    }
    return (!isValid.none());
}

bool isInsideRing(const Board &b, positionT p) {
    if (!b.G.occupied.get_bit(p)) {
        bool full = true;
        for (int d = 0; d < 6; d++) {
            if (!b.G.occupied.get_bit((p + dirDif[d]) & 1023)) {
                full = false;
                break;
            }
        }
        if (full) return true;
    }
    return false;
}

// =====================================================
// EVALUATION FUNCTIONS
// =====================================================

#include <immintrin.h>

double DynEval::actionsScore(const Board & b, pieceT myQueen, positionT oppositeQueenPosition)const {
    // Per ogni bug, valutiamo le azioni che può fare
    int numberOfAroundQueen = isQueenAlmostSurrounded(b, oppositeQueenPosition);
    bool canWinInOneMove = (numberOfAroundQueen == 5);
    double score=0;
    double actBug[32];
    for(int i=0;i<32;i++){
        actBug[i]=0;
    }
    bool noisy = false;
    for(int i=0;i<b.numAction;i++){
        actionT action=b.resAction[i];
        pieceT bug=action%32;
        positionT destPos=action>>5;
        actBug[bug]++;

        if (isAdjacent(destPos, oppositeQueenPosition)) {
            score += W.noisyWeight(kind(bug));
            noisy = true;
        } else {
            score += W.quietWeight(kind(bug));
        }
    }

    if (canWinInOneMove && noisy) {
        return 1e10;
    }

    for(int i=1;i<=28;i++){
        if(actBug[i] == 0){
            score+=W.isPinnedWeight(kind(i));
        }
    }

    return score;
}


double DynEval::positionalScore(const Board &b, PlayerColor currentColor, PlayerColor oppositeColor)const {
    // Valutiamo la posizione del bug, se è coperto, vicinato..
    double score=0;

    alignas(64) uint8_t nS[1024];
    for(int i=0;i<1024;i++) {
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
            bugScore[p] = W.placedWeight(kind(p));
            positionT currPos=b.G.getPosition(p);

            if(b.G.topPiece(currPos)!=p){
                bugScore[p]+=W.isCoveredWeight(kind(p));
            } else {
                bugScore[p]+=W.isOnTopWeight(kind(p));
            }
            
            bugScore[p]+=((double)(nS[currPos]>>3))*W.enemyCloseWeight(kind(p))+
                    ((double)((nS[currPos]>>3)+(nS[currPos]&7)))*W.friendCloseWeight(kind(p));
        }
    }
    
    for(int i=baseMy;i<=baseMy+13;i++)score+=bugScore[i];
    for(int i=baseOpp;i<=baseOpp+13;i++)score-=bugScore[i];
    
    return score;
}

double DynEval::strategicScore(const Board &b, PlayerColor color, positionT queenPos) const {
    double score = 0;
    int base = (color == PlayerColor::WHITE) ? 1 : 15;

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
                    if (kind(top) == SOLDIER_ANT || kind(top) == BEETLE || kind(top) == PILLBUG) {
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

    return score;
}

double DynEval::topologyScore(const Board &b, PlayerColor color, positionT queenPos, positionT otherQueenPos) const {
    double score = 0;

    int surrounding = isQueenAlmostSurrounded(b, queenPos);
    score -= pow(W.toExponentiate, surrounding);

    int surroundingEnemy = isQueenAlmostSurrounded(b, otherQueenPos);
    score += pow(W.toExponentiate, surroundingEnemy);

    if (isGateFormationAroundQueen(b, queenPos)) {
        score += W.queenGateDefenseBonus();
    }

    // TEMPO: 
    int baseMy=(color==PlayerColor::WHITE?1:15);
    int baseOpp=16-baseMy;
    for(int i=baseMy;i<=baseMy+13;i++) {
        if (distanceToQueen(b.G.getPosition(i), queenPos) < 3) {
            score += W.defensiveWeight(kind(i));
        }
        if (distanceToQueen(b.G.getPosition(i), otherQueenPos) < 3) {
            score += W.offensiveWeight(kind(i));
        }
    }
    for(int i=baseOpp;i<=baseOpp+13;i++) {
        if (distanceToQueen(b.G.getPosition(i), otherQueenPos) < 3) {
            score -= W.defensiveWeight(kind(i));
        }
        if (distanceToQueen(b.G.getPosition(i), queenPos) < 3) {
            score -= W.offensiveWeight(kind(i));
        }
    }

    return score;
}

double DynEval::evalBoardCurrentPlayer(Board & b) const{

    if(b.getGameState()>1){// implement suicide
        if ((b.getGameState()==BLACK_WIN && b.currentColor()==PlayerColor::BLACK) || (b.getGameState()==WHITE_WIN && b.currentColor()==PlayerColor::WHITE)){
            return 1e10;
        } 
        if (b.getGameState() == DRAW) return 0;
        return -1e10; 
    }
    b.ComputePossibleMoves();

    PlayerColor color = b.currentColor();
    PlayerColor otherColor = (color == PlayerColor::WHITE) ? PlayerColor::BLACK : PlayerColor::WHITE;
    int myQueen = (color == PlayerColor::WHITE) ? 8 : 22;
    positionT queenPos = b.G.getPosition(myQueen);
    int otherQueen = (color == PlayerColor::WHITE) ? 22 : 8;
    positionT otherQueenPos = b.G.getPosition(otherQueen);

    double score=0;
    score += actionsScore(b, myQueen, otherQueenPos);  // for each action, compute the utility of the action
    score += positionalScore(b, color, otherColor);  // for each piece, compute the utility based on neighbor
    score += strategicScore(b, color, queenPos);   // for some piece, if they are close to the queen can be helpful
    score += topologyScore(b, color, queenPos, otherQueenPos);   // find if there are rings or other 

    return score;
}