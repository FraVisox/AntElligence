#include "enums.h"
#ifndef WEIGHTS_HELP_H
#define WEIGHTS_HELP_H
class WeightsHelper{
    private:
        double weightVector[42];
    public: 
    WeightsHelper();
    const void initDefaultWeights();

    const double QueenMoveWeight() const ;
    const double numActionWeight() const;
    const double noisyWeight(BugType) const ;
    const double converedWeight(BugType) const;
    const double numEnemyCloseWeight(BugType)const;
    const double totalNumCloseWeight(BugType)const;
    const double placedWeight(BugType) const;
};
extern double startGame[8][7];

extern  double endgame[8][7];


double getWeight(BugType bt,int,int=0);

#endif