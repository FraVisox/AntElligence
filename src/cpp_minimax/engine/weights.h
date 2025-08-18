#include "enums.h"
#ifndef WEIGHTS_HELP_H
#define WEIGHTS_HELP_H
class WeightsHelper{
    public:
        const int numberOfWeights=84;
        alignas(32) int weightVector[84];
     
        WeightsHelper();
        const void initDefaultWeights();

        const  int placedWeight(BugType) const;
        const  int isPinnedWeight(BugType) const;
        const  int isCoveredWeight(BugType) const;
        const  int noisyWeight(BugType) const ;
        const  int quietWeight(BugType) const ;
        const  int friendCloseWeight(BugType)const;
        const  int enemyCloseWeight(BugType)const;
        const int isOnTopWeight(BugType) const ;
        const int offensiveWeight(BugType) const ;
        const int defensiveWeight(BugType) const ;

        const int pillbugNearQueenBonus() const;
        const int mosquitoNearStrongBugBonus() const;
        const int spiderDistancePenalty() const;
        const int queenGateDefenseBonus() const;

        const double toExponentiate = 12.0;

};

#endif