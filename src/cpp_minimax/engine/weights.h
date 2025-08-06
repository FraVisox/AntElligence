#include "enums.h"
#ifndef WEIGHTS_HELP_H
#define WEIGHTS_HELP_H
class WeightsHelper{
    public:
        alignas(32)double weightVector[48];
        alignas(32) double explicitWeight[168];
     
    WeightsHelper();
    const  double numActionWeight(BugType) const;
    const int numActOffset;
    const  double noisyWeight(BugType) const ;
    const int noisyOffset;
    const  double converedWeight(BugType) const;
    const int converedOffset;
    const  double numEnemyCloseWeight(BugType)const;
    const int numEnemyCloseOffset;
    const  double totalNumCloseWeight(BugType)const;
    const int totalNumCloseOffset;
    const  double placedWeight(BugType) const;
    const int placedOffset;
};
extern double startGame[8][7];

extern  double endgame[8][7];


double getWeight(BugType bt,int,int=0);

#endif