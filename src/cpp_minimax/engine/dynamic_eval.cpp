#include "board.h"
#include "weights.h"
#include "dynamic_eval.h"
#include "enums.h"
#include <chrono>

bool isEnemyPiece(pieceT p, PlayerColor me) {
    if (me == PlayerColor::WHITE) return p >= 15 && p <= 28;
    else return p >= 1 && p <= 14;
}

bool isMyPiece(pieceT p, PlayerColor me) {
    return !isEnemyPiece(p, me);
}

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
    // === New: Ladybug can reach normally inaccessible pockets or ring interiors
    int ladybugsIntoPockets = 0;
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

        // === New: Add a weight if the bug moves away from our queen
        if (isAdjacent(b.G.getPosition(bug), b.G.getPosition(myQueen)) && !isAdjacent(destPos, b.G.getPosition(myQueen))) {
            score += W.movedAwayFromQueenWeight();
        }

        if (kind(bug) != LADYBUG) continue;

        // Check if destination is a pocket or inside a ring
        int neighbors = 0;
        for (int d = 0; d < 6; d++) {
            if (b.G.occupied.get_bit((destPos + dirDif[d]) & 1023))
                neighbors++;
        }

        if (neighbors >= 4 || isInsideRing(b, destPos)) {
            ladybugsIntoPockets++;
        }
    }

    if (ladybugsIntoPockets > 0) {
        score += ladybugsIntoPockets * W.ladybugPocketBonus();
    }

    for(int i=0;i<32;i++){
        if(actBug[i] == 0){
            score+=W.isPinnedWeight(kind(i));
        }
    }


    // === New: Unpinned ants
    int base = (b.currentColor() == PlayerColor::WHITE) ? 1 : 15;
    for (int i = base; i <= base + 13; i++) {
        if (b.G.isPlaced[i] && kind(i) == SOLDIER_ANT && actBug[i] > 0) {
            score += W.unpinnedAntBonus();
            break;
        }
    }

    // === New: Add a weight if the queen can escape
    if(actBug[myQueen] != 0){
        score+=W.queenCanEscape();
    }

    positionT queenPos = b.G.getPosition(myQueen);

    // === New: Our Hopper can hop out from near our queen
    int hoppersCanEscape = 0;
    for (int i = base; i <= base + 13; i++) {
        if (!b.G.isPlaced[i] || kind(i) != GRASSHOPPER) continue;
        positionT pos = b.G.getPosition(i);

        if (isAdjacent(pos, queenPos) && actBug[i] > 0) {
            hoppersCanEscape++;
        }
    }
    if (hoppersCanEscape > 0) {
        score += hoppersCanEscape * W.hopperEscapeBonus();
    }

    // === New: ring traps us or the opponent?
    PlayerColor color = b.currentColor();
    if (detectRingFormation(b)) {
        int myFreed = 0;
        for (int p = 1; p <= 28; p++) {
            if (!b.G.isPlaced[p]) continue;
            positionT pos = b.G.getPosition(p);
            if (isInsideRing(b, pos)) {
                if (isMyPiece(p, color) && actBug[p] > 0) myFreed++;
            }
        }
        score += (myFreed) * W.ringFreeTrapBonus();
        score -= W.ringPenalty();
    }

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
    
    return score;
}

double DynEval::strategicScore(const Board &b) const {
    double score = 0;
    PlayerColor color = b.currentColor();
    int base = (color == PlayerColor::WHITE) ? 1 : 15;
    positionT queenPos = b.G.getPosition(base + 7); // Queen index is 8 or 22


    // === New: Beetle on top of opponent’s Queen or key attacker (ANT) ===
    int myBeetlesOnKey = 0;
    int oppBeetlesOnKey = 0;
    int oppQueen = (color == PlayerColor::WHITE) ? 22 : 8;
    int myQueen = (color == PlayerColor::WHITE) ? 8 : 22;
    auto isKeyAttacker = [&](pieceT bug) {
        int k = kind(bug);
        return (k == SOLDIER_ANT || k == QUEEN);
    };

    // My Beetles
    for (int i = 0; i < 32; i++) {
        if (!b.G.isPlaced[i] || kind(i) != BEETLE) {
            continue;
        }
        positionT pos = b.G.getPosition(i);
        pieceT top = b.G.topPiece(pos);
        PlayerColor bugColor = i / 16 ? PlayerColor::BLACK : PlayerColor::WHITE;
        if (top == i) {
            vector<pieceT> bugs = b.G.getBugs(pos);
            for (int j = 0; j < bugs.size(); j++) {
                if (isKeyAttacker(bugs[j]) && bugs[j] / 16 != bugColor) {
                    if (bugColor == b.currentColor()) {
                        myBeetlesOnKey++;
                    } else {
                        oppBeetlesOnKey++;
                    }
                    break;
                }
            }
        }
    }

    score += (myBeetlesOnKey - oppBeetlesOnKey) * W.beetleOnKeyBonus();

    // === New: Spider elbows
    int spiderElbows = 0;
    for (int i = base; i <= base + 13; i++) {
        if (!b.G.isPlaced[i] || kind(i) != SPIDER) continue;
        positionT pos = b.G.getPosition(i);
        for (int d = 0; d < 6; d++) {
            positionT left = (pos + dirDif[d]) & 1023;
            positionT right = (pos + dirDif[(d+1)%6]) & 1023;
            positionT corner = (pos + dirDif[(d+5)%6]) & 1023; // inside of the elbow

            if (b.G.occupied.get_bit(left) && b.G.occupied.get_bit(right) &&
                !b.G.occupied.get_bit(corner)) {
                // Bonus if the corner hex is next to an enemy piece (immobilizing)
                for (int dd = 0; dd < 6; dd++) {
                    positionT adj = (corner + dirDif[dd]) & 1023;
                    if (b.G.occupied.get_bit(adj)) {
                        pieceT top = b.G.topPiece(adj);
                        if ((color == PlayerColor::WHITE && top >= 15) ||
                            (color == PlayerColor::BLACK && (top >= 1 && top <= 14))) {
                            spiderElbows++;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (spiderElbows > 0) {
        score += spiderElbows * W.spiderElbowBonus();
    }

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
            // === New: Mosquito near pillbug and queen is better
            bool nearPill = false;
            bool nearQueen = false;
            for (int d = 0; d < 6; d++) {
                positionT neighbor = (pPos + dirDif[d]) & 1023;
                if (b.G.occupied.get_bit(neighbor)) {
                    pieceT top = b.G.topPiece(neighbor);
                    if (kind(top) == SOLDIER_ANT || kind(top) == BEETLE || kind(top) == PILLBUG) {
                        score += W.mosquitoNearStrongBugBonus();
                        if (kind(top) == PILLBUG) {
                            nearPill = true;
                        }
                    }
                    else if (kind(top) == QUEEN) {
                        nearQueen = true;
                    }
                }
            }
            if (nearPill && nearQueen) {
                score += W.mosquitoNearPillbugNearQueenBonus();
            }
        }
        else if (k == SPIDER) {
            int dist = distanceToQueen(pPos, queenPos);
            score -= dist * W.spiderDistancePenalty();
        }
    }

    // === New: Checkmate faster
    auto minMovesToMate = [&](PlayerColor side) {
        positionT oppQ = (side == PlayerColor::WHITE) ? b.G.getPosition(22) : b.G.getPosition(8);
        int emptyAdj = 0;
        for (int d = 0; d < 6; d++) {
            if (!b.G.occupied.get_bit((oppQ + dirDif[d]) & 1023))
                emptyAdj++;
        }
        if (emptyAdj == 0) return INT_MAX; // already surrounded

        // Very rough: take the closest piece (in hex distance) that can legally move
        int baseIdx = (side == PlayerColor::WHITE) ? 1 : 15;
        int minDist = INT_MAX;
        for (int p = baseIdx; p <= baseIdx + 13; p++) {
            if (!b.G.isPlaced[p]) continue;
            int dist = distanceToQueen(b.G.getPosition(p), oppQ) - 1;
            if (dist < minDist) minDist = dist;
        }
        return minDist;
    };

    int myMateDist = minMovesToMate(color);
    PlayerColor oppositeColor = (color == PlayerColor::WHITE) ? PlayerColor::BLACK : PlayerColor::WHITE;
    int oppMateDist = minMovesToMate(oppositeColor);

    if (myMateDist < oppMateDist) {
        score += (oppMateDist - myMateDist) * W.fasterMateBonus();
    }


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


    // TODO: Bonus for controlling gates leading to opponent’s Queen
    // Bonus if a ring formation frees your pieces while trapping opponent’s.
    // Compare total number of mobile attackers each side has; score proportional advantage.
    // Bonus if you can deliver checkmate faster than opponent based on bug mobility.

    return score;
}



double DynEval::evalBoardCurrentPlayer(Board & b) const{

    // Measure the time
    auto start = std::chrono::high_resolution_clock::now();


    if(b.getGameState()>1){// implement suicide
        if (b.getGameState()==BLACK_WIN && b.currentColor()==PlayerColor::BLACK || b.getGameState()==WHITE_WIN && b.currentColor()==PlayerColor::WHITE){
            return 1e10;
        } 
        if (b.getGameState() == DRAW) return 0;
        return -1e10; 
    }
    b.ComputePossibleMoves();

    double score=0;
    score += actionsScore(b);  // for each action, compute the utility of the action
    score += positionalScore(b);  // for each piece, compute the utility based on neighbor
    score += strategicScore(b);   // for some piece, if they are close to the queen can be helpful
    score += topologyScore(b);   // find if there are rings or other 

    auto end = std::chrono::high_resolution_clock::now();

    clog << "Time to evaluate one board: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;
    
    return score;
}