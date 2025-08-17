#include "board.h"
#include "weights.h"

class DynEval{
    private:
    WeightsHelper W;
    double topologyScore(const Board &) const;
    double strategicScore(const Board &) const;
    double positionalScore(const Board &) const;
    double actionsScore(const Board &) const;

public:

    double evalBoardCurrentPlayer(Board &) const;
};