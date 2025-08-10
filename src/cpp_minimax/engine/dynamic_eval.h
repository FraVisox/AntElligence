#include "board.h"
#include "weights.h"

class DynEval{
    private:
    WeightsHelper W;
    double topologyScore(const Board &) const;
    double strategicScore(const Board &) const;
    double positionalScore(const Board &) const;
    double actionsScore(const Board &) const;

    static inline bool isAdjacent(positionT a, positionT b) {
        for (int i = 0; i < 6; i++) {
            if (((a + dirDif[i]) & 1023) == (b & 1023)) return true;
        }
        return false;
    }

    static int distanceToQueen(positionT a, positionT b) {
        // Approximate distance on hex grid (valid since it's small)
        int dx = ((a & 31) - (b & 31));
        int dy = ((a >> 5) - (b >> 5));
        return abs(dx) + abs(dy);
    }

    static bool isGateFormationAroundQueen(const Board& b, positionT qPos) {
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

    static int isQueenAlmostSurrounded(const Board &b, positionT qPos) {
        int count = 0;
        for (int i = 0; i < 6; i++) {
            positionT n = (qPos + dirDif[i]) & 1023;
            if (b.G.occupied.get_bit(n)) count++;
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

public:

    double evalBoardCurrentPlayer(Board &) const;
};