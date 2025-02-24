#ifndef STRATEGY_H
#define STRATEGY_H

#include "engine/enums.h"

class Strategy {
    public:
        StrategyName name;
    
        Strategy(StrategyName n) : name(n) {}
}; //TODO: implement

#endif