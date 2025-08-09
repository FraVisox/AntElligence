#include "enums.h"
#ifndef WEIGHTS_HELP_H
#define WEIGHTS_HELP_H
class WeightsHelper{
    public:
        alignas(32) int weightVector[72];
     
        WeightsHelper();
        const void initDefaultWeights();

        const  int placedWeight(BugType) const;
        const int placedOffset;

        const  int isPinnedWeight(BugType) const;
        const int isPinnedOffset;

        const  int isCoveredWeight(BugType) const;
        const int isCoveredOffset;

        const  int noisyWeight(BugType) const ;
        const int noisyOffset;

        const  int quietWeight(BugType) const ;
        const int quietOffset;

        const  int friendCloseWeight(BugType)const;
        const int friendCloseOffset;

        const  int enemyCloseWeight(BugType)const;
        const int enemyCloseOffset;

        const int pillbugNearQueenBonus() const;
        const int mosquitoNearStrongBugBonus() const;
        const int spiderDistancePenalty() const;
        const int queenGateDefenseBonus() const;
        const int ringPenalty() const;
        const int queenCanEscape() const;
        const int beetleOnKeyBonus() const;
        const int blockingQueenSpotBonus() const;
        const int unpinnedAntBonus() const;
        const int spiderElbowBonus() const;
        const int hopperEscapeBonus() const;
        const int ladybugPocketBonus() const;
        const int movedAwayFromQueenWeight() const;
        const int ringFreeTrapBonus() const;
        const int mosquitoNearPillbugNearQueenBonus() const;
        const int fasterMateBonus() const;

};
extern double startGame[8][7];

extern  double endgame[8][7];


double getWeight(BugType bt,int,int=0);

#endif