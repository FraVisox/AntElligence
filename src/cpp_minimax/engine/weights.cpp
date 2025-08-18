#include "weights.h"
#include "piece.h"
#include <iostream>
#include <fstream>
using namespace std;


WeightsHelper::WeightsHelper() {
    fstream file;
    file.open("weights.txt", ios::in);
    if (file.is_open()) {
        for (int i = 0; i < 84; i++) {
            file >> weightVector[i];
        }
    } else {
        initDefaultWeights();
    }
}

const void WeightsHelper::initDefaultWeights() {

  int hv[84] = {
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

  // IsOnTopWeight
  0,
  50000,
  0,
  90000,
  120000,
  45000,
  0,
  25,

  // OffensiveWeight
  50000,
  100000,
  30000,
  -45,
  3000,
  45000,
  70000,
  25,

  // DefensiveWeight
  5000,
  50000,
  43000,
  0,
  3400,
  100000,
  56000,
  250000,

    // Others
  1500, //pillbugNearQueenBonus
  1000, //mosquitoNearStrongBugBonus
  1000, //spiderDistancePenalty
  40000, //queenGateDefenseBonus
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

const int WeightsHelper::isOnTopWeight(BugType bt) const {
  return weightVector[56+bt];
}

const int WeightsHelper::offensiveWeight(BugType bt) const {
  return weightVector[64+bt];
}

const int WeightsHelper::defensiveWeight(BugType bt) const {
  return weightVector[72+bt];
}

const int WeightsHelper::pillbugNearQueenBonus() const {
  return weightVector[80];
}

const int WeightsHelper::mosquitoNearStrongBugBonus() const {
  return weightVector[81];
}

const int WeightsHelper::spiderDistancePenalty() const {
  return weightVector[82];
}

const int WeightsHelper::queenGateDefenseBonus() const {
  return weightVector[83];
}