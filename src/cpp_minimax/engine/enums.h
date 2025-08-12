#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <cstdint>
using namespace std;
typedef uint16_t actionT;
const  int MAX_ACTIONS_SIZE=256;



//Color of the player

enum PlayerColor{
    WHITE=0,
    BLACK=1
};

string ColorToCompleteString(PlayerColor c);

int opposite(int x);

PlayerColor parseColor(string s);

//Game state


enum GameState{   
    NOT_STARTED=0,
    STARTED=1,
    WHITE_WIN=2,
    BLACK_WIN=3,
    DRAW=4,
};

enum GameType{
    Base=0,
    Base_M=1,
    Base_L=2,
    Base_P=3,
    Base_ML=4,
    Base_MP=5,
    Base_LP=6,
    Base_MLP=7
};

string GameStateToString(GameState gs) ;



string GameTypeToString(GameType gt);





enum BugType{
    SPIDER=0,
    BEETLE=1,
    GRASSHOPPER=2,
    QUEEN=3,
    SOLDIER_ANT=4,
    MOSQUITO=5,
    LADYBUG=6,
    PILLBUG=7
};


enum class StrategyName {
    RANDOM = 0,
    MINIMAX = 1,
    DRL = 2
};


BugType parseBugType(string s);


enum class StrategyName;


#define InPlayWeight 0
#define IsPinnedWeight 1 
#define IsCoveredWeight 2
#define NoisyMoveWeight 3
#define QuietMoveWeight 4
#define FriendlyNeighborWeight 5
#define EnemyNeighborWeight 6




#endif