#include "weights.h"
#include "piece.h"
#include <iostream>
#include <fstream>
using namespace std;


WeightsHelper::WeightsHelper(): placedOffset(0), isPinnedOffset(1), isCoveredOffset(2), noisyOffset(3), quietOffset(4), friendCloseOffset(5), enemyCloseOffset(6) {
    initDefaultWeights();
}

const void WeightsHelper::initDefaultWeights() {

  int hv[60] = {
  // InPlayWeight
  25436,
  -47942,
  62698,
  13433,
  132380,
  35702,
  428,
  -3,

  // IsPinnedWeight
  -46539,
  -48021,
  -82598,
  -129954,
  -51036,
  -47853,
  428,
  36,

  // IsCoveredWeight
  7318,
  9325,
  24024,
  -20359,
  -1515,
  -5471,
  1,
  1,

  // NoisyMoveWeight
  139756,
  142117,
  105511,
  72291,
  53867,
  34855,
  -327,
  -327,

  // QuietMoveWeight
  -20696,
  11689,
  -12497,
  95775,
  4129,
  3658,
  -2037,
  -5,

  // FriendlyNeighborWeight
  21040,
  36463,
  13656,
  -317467,
  14307,
  9199,
  426,
  1,

  // EnemyNeighborWeight
  47143,
  63937,
  24316,
  -538149,
  21240,
  20205,
  0,
  25,

  // Others
  1500, //pillbugNearQueenBonus
  1000, //mosquitoNearStrongBugBonus
  1000, //spiderDistancePenalty
  40000 //queenGateDefenseBonus
  }; 

  for(int i=0;i<numberOfWeights;i++){
    weightVector[i]=hv[i];
  }
}



const  int WeightsHelper::placedWeight(BugType bt) const{
  return weightVector[0+bt];
}
const  int WeightsHelper::isPinnedWeight(BugType bt) const {
  return weightVector[8+bt];
}
const  int WeightsHelper::isCoveredWeight(BugType bt ) const{
  return weightVector[16+bt];
}
const  int WeightsHelper::noisyWeight(BugType bt)const{
  return weightVector[24+bt];
}
const  int WeightsHelper::quietWeight(BugType bt)const{
  return weightVector[32+bt];
}
const  int WeightsHelper::friendCloseWeight(BugType bt) const{
  return weightVector[40+bt];
}

const int WeightsHelper::enemyCloseWeight(BugType bt) const{
  return weightVector[48+bt];
}

const int WeightsHelper::pillbugNearQueenBonus() const {
  return weightVector[56];
}

const int WeightsHelper::mosquitoNearStrongBugBonus() const {
  return weightVector[57];
}

const int WeightsHelper::spiderDistancePenalty() const {
  return weightVector[58];
}

const int WeightsHelper::queenGateDefenseBonus() const {
  return weightVector[59];
}