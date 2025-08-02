#include "board.h"
#include "weights.h"

class DynEval{
    private:
    const WeightsHelper W;
    PlayerColor currentColor;
    PlayerColor oppositeColor;
    pieceT myQueen;
    pieceT oppositeQueen;
    positionT myQueenPosition;
    positionT oppositeQueenPosition;
    double topologyScore(const Board &) const;
    double strategicScore(const Board &) const;
    double positionalScore(const Board &) const;
    double actionsScore(const Board &) const;
public:

    double evalBoardCurrentPlayer(Board &);
};