#include "board.h"
#include "weights.h"

class DynEval{
    private:
    WeightsHelper W;
    double topologyScore(const Board &, PlayerColor color, positionT queenPos, positionT otherQueenPos) const;
    double strategicScore(const Board &, PlayerColor color, positionT queenPos) const;
    double positionalScore(const Board &, PlayerColor currentColor, PlayerColor oppositeColor) const;
    double actionsScore(const Board &, pieceT myQueen, positionT oppositeQueenPosition) const;

public:

    double evalBoardCurrentPlayer(Board &) const;
};