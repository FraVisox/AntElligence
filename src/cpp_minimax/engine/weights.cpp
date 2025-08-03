#include "weights.h"
#include <iostream>
using namespace std;

WeightsHelper::WeightsHelper(){
  double hv[42]={
    130,                // QueenMoveWeight
    80,                // numActionWeight
//  S     B   G    Q    A    M    L    P 
    4,  20,  15,   5,  30,  10,   5,   5,  // noisyWeight
   -5, -10,  -5,-120, -30, -15,  -7, -10,  // converedWeight
   -1,   1,  -4, -80,  -5,  -7,  -2,   0,  // numEnemyCloseWeight
    0,   0,   4, -60,   0,  20,   4,   5,  // totalNumCloseWeight
    5,  10,   8,  60,  10,   5,  20,   4   // placedWeight
  };
  for(int i=0;i<42;i++){
    weightVector[i]=hv[i];
  }
}


const double WeightsHelper::QueenMoveWeight() const {
  return weightVector[0];
}
const double WeightsHelper::numActionWeight() const{
  return weightVector[1];
}
const double WeightsHelper::noisyWeight(BugType bt) const {
  return weightVector[2+bt];
}
const double WeightsHelper::converedWeight(BugType bt ) const{
  return weightVector[10+bt];
}
const double WeightsHelper::numEnemyCloseWeight(BugType bt)const{
  return weightVector[18+bt];
}
const double WeightsHelper::totalNumCloseWeight(BugType bt)const{
  return weightVector[26+bt];
}
const double WeightsHelper::placedWeight(BugType bt) const{
  return weightVector[34+bt];
}
  

double startGame[8][7]={
    {//QueenBee
    63558.424116300164,    //InPlayWeight
    -16068.975008474492,   //IsPinnedWeight
    -10847.56765389993,    //IsCoveredWeight
    -27548.36384306823,    //NoisyMoveWeight
    -58684.53995399401,    //QuietMoveWeight
    6083.816305341258,     //FriendlyNeighborWeight
    -16777.90671222724,    //EnemyNeighborWeight
    },
  {//Spider
    -78668.57111290144,     //InPlayWeight
    -40182.83585644461,     //IsPinnedWeight
    3522.4688484300345,     //IsCoveredWeight
    96533.76080639324,      //NoisyMoveWeight
    24696.29074524807,      //QuietMoveWeight
    -38936.63081162773,     //FriendlyNeighborWeight
    -23840.635072629706,    //EnemyNeighborWeight
  },
  {//Beetle
    -292118.4826934236,     //InPlayWeight
    42682.09004360313,      //IsPinnedWeight
    4481.178845826221,      //IsCoveredWeight
    17665.014704305027,     //NoisyMoveWeight
    -30118.546365364382,    //QuietMoveWeight
    -330519.2236168998,     //FriendlyNeighborWeight
    -15571.384868146333,    //EnemyNeighborWeight
  },{//Grasshopper
    -62143.66709528388,     //InPlayWeight>
    -37382.75816433833,     //IsPinnedWeight>
    6.7071386644275,        //IsCoveredWeight>
    4.27711240177,          //NoisyMoveWeight>
    5.887177023414,         //QuietMoveWeight>
    -46514.16457826511,     //FriendlyNeighborWeight>
    -29599.124484381024,    //EnemyNeighborWeight>
  },{//SoldierAnt
    150335.73123164993,     //InPlayWeight
    -54347.58400587852,     //IsPinnedWeight
    2181.3845855769014,     //IsCoveredWeight
    -12190.782082844375,    //NoisyMoveWeight
    -2768.31388448452,      //QuietMoveWeight
    -24781.768062011586,    //FriendlyNeighborWeight
    -124949.7080146349,     //EnemyNeighborWeight
  },{
      147014.737145555,    // Mosquito.InPlayWeight>
      -2891.0586433755066,    // Mosquito.IsPinnedWeight>
      274.477890120718,    // Mosquito.IsCoveredWeight>
      7418.816073993939,    // Mosquito.NoisyMoveWeight>
      -2217.07900807487,    // Mosquito.QuietMoveWeight>
      5877.374514571279,    // Mosquito.FriendlyNeighborWeight>
      12195.85956817655,    // Mosquito.EnemyNeighborWeight>
},{//Ladybug
    2403.5193930400983,     //InPlayWeight
    2403.5193930400983,     //IsPinnedWeight
    0.12466972517998409,   //Pillbug.IsCoveredWeight>
    -176.46003344968662,   //Pillbug.NoisyMoveWeight>
    -11221.051806351792,    //QuietMoveWeight
    2385.4588234124367,     //FriendlyNeighborWeight
    1410.332157926724,      //EnemyNeighborWeight
    },{
      -9.113687271307544,   //Pillbug.InPlayWeight>
      1486.6262842915492,   //Pillbug.IsPinnedWeight>
      0.12466972517998409,   //Pillbug.IsCoveredWeight>
      -176.46003344968662,   //Pillbug.NoisyMoveWeight>
      -132.41559439730622,   //Pillbug.QuietMoveWeight>
      24.673845413636343,   //Pillbug.FriendlyNeighborWeight>
      851.9075292842033,   //Pillbug.EnemyNeighborWeight>
    }
      
};

double endgame[8][7]={
    {
  25435.882238354585,        //Spider.InPlayWeight
  -46539.05096879799,        //Spider.IsPinnedWeight
  7318.062445533153,         //Spider.IsCoveredWeight
  139756.22020687832,        //Spider.NoisyMoveWeight
  -20695.847798165196,       //Spider.QuietMoveWeight
  21040.37841289118,         //Spider.FriendlyNeighborWeight
  47143.13584635398,         //Spider.EnemyNeighborWeight
},{
  -47942.30787762037,        //Beetle.InPlayWeight
  -48021.26522496219,        //Beetle.IsPinnedWeight
  9325.20680229596,          //Beetle.IsCoveredWeight
  142117.44307757894,        //Beetle.NoisyMoveWeight
  11689.30208954932,         //Beetle.QuietMoveWeight
  36462.79568546643,         //Beetle.FriendlyNeighborWeight
  63931.69017718444,         //Beetle.EnemyNeighborWeight
},{
  62698.37281422093,         //Grasshopper.InPlayWeight
  -82598.03083886552,        //Grasshopper.IsPinnedWeight
  24024.225201639405,        //Grasshopper.IsCoveredWeight
  105510.87832342245,        //Grasshopper.NoisyMoveWeight
  -12497.189426607008,       //Grasshopper.QuietMoveWeight
  13656.02929100097,         //Grasshopper.FriendlyNeighborWeight
  24316.040387962978,        //Grasshopper.EnemyNeighborWeight
},{
  13432.951885680533,        //QueenBee.InPlayWeight
  -129954.41883899852,       //QueenBee.IsPinnedWeight
  -20359.393475776742,       //QueenBee.IsCoveredWeight
  72291.27947098055,         //QueenBee.NoisyMoveWeight
  95774.84135111222,         //QueenBee.QuietMoveWeight
  -317466.59939896787,       //QueenBee.FriendlyNeighborWeight
  -538148.8342704792,        //QueenBee.EnemyNeighborWeight
},{
  132380.04004659085,        //SoldierAnt.InPlayWeight
  -51035.8531595224,         //SoldierAnt.IsPinnedWeight
  -1515.3280863367781,       //SoldierAnt.IsCoveredWeight
  53866.8750489757,          //SoldierAnt.NoisyMoveWeight
  4129.437215966653,         //SoldierAnt.QuietMoveWeight
  14307.12578295306,         //SoldierAnt.FriendlyNeighborWeight
  21240.12696300159,         //SoldierAnt.EnemyNeighborWeight
},{
      35702.098965012,  //Mosquito.InPlayWeight>
      -47853.25550138967,  //Mosquito.IsPinnedWeight>
      -5471.658340342187,  //Mosquito.IsCoveredWeight>
      34855.368897836735,  //Mosquito.NoisyMoveWeight>
      3658.3069258791934,  //Mosquito.QuietMoveWeight>
      9198.549211028952,  //Mosquito.FriendlyNeighborWeight>
      20205.426928489276,  //Mosquito.EnemyNeighborWeight>
},{
  428.1203911929252,         //Ladybug.InPlayWeight
  428.1203911929252,         //Ladybug.IsPinnedWeight
      1.0348840069979819,  //Pillbug.IsCoveredWeight>
      -327.45214315771926,  //Pillbug.NoisyMoveWeight>
  -2037.2972162731394,       //Ladybug.QuietMoveWeight
  426.07017642756523,        //Ladybug.FriendlyNeighborWeight
  0
},{
      -2.92230300211222,  //Pillbug.InPlayWeight>
      36.14230688121976,  //Pillbug.IsPinnedWeight>
      1.0348840069979819,  //Pillbug.IsCoveredWeight>
      -327.45214315771926,  //Pillbug.NoisyMoveWeight>
      -4.7686213002021525,  //Pillbug.QuietMoveWeight>
      0.7586273420223163,  //Pillbug.FriendlyNeighborWeight>
      24.950130621306656,  //Pillbug.EnemyNeighborWeight>
}};

/*


    
double startGame[8][7]={
    {1,0,0,0, 0, 0   ,0 },    // SPIDER=0,
    {1,0,0,0, 0, 0   ,0 },    //  BEETLE=1,
    {1,0,0,0, 0, 0   ,0 },    //  GRASSHOPPER=2,
    {1,0,0,0, 0, -10 ,-10},    //  QUEEN=3,
    {1,0,0,0, 0, 0   ,0 },    //  SOLDIER_ANT=4,
    {1,0,0,0, 0, 0   ,0 },    //  MOSQUITO=5,
    {1,0,0,0, 0, 0   ,0 },    //  LADYBUG=6,
    {1,0,0,0, 0, 0   ,0 }     // PILLBUG=7
  // 0 1 2 3  4  5    6
};

double endgame[8][7]={

    {
    0,    //InPlayWeight
    0,   //IsPinnedWeight
    0,    //IsCoveredWeight
    0,    //NoisyMoveWeight
    0,    //QuietMoveWeight
    -0.8,     //FriendlyNeighborWeight
    -1,    //EnemyNeighborWeight
    },
    {1,0,0,0, 0, 0   ,0 },
    {1,0,0,0, 0, 0   ,0 },
    {1,0,0,0, 0, -8   ,-10},
    {1,0,0,0, 0, 0   ,0 },
    {1,0,0,0, 0, 0   ,0 },
    {1,0,0,0, 0, 0   ,0 },
    {1,0,0,0, 0, 0   ,0 }
};
*/
// InPlayWeight 0
// IsPinnedWeight 1 
// IsCoveredWeight 2
// NoisyMoveWeight 3
// QuietMoveWeight 4
// FriendlyNeighborWeight 5
// EnemyNeighborWeight 6


double getWeight(BugType type, int key,int gamekind) {
    double v=0;
      v=startGame[type][key];
    return v;
      v=endgame[type][key];
    return v;
}


