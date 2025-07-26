#ifndef ENUMS_H
#define ENUMS_H

#include <string>
#include <cstdint>
using namespace std;
typedef uint16_t actionT;
const  int MAX_ACTIONS_SIZE=512;



//Color of the player

enum PlayerColor{
    WHITE=0,
    BLACK=1
};

string ColorToCompleteString(PlayerColor c) {
    if (c == PlayerColor::WHITE) return "White";
    return "Black";
}

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

string GameStateToString(GameState gs) {
    switch (gs)
    {
        case NOT_STARTED:
            return "NotStarted";
        case STARTED:
            return "InProgress";
        case WHITE_WIN:
            return "WhiteWins";
        case BLACK_WIN:
            return "BlackWins";
        case DRAW:
            return "Draw";
    }
    throw "Invalid game state";
}


//Game type

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

string GameTypeToString(GameType gt){
    switch (gt)
    {
        case Base:
            return "Base";
        case Base_L:
            return "Base+L";
        case Base_M:
            return "Base+M";
        case Base_P:
            return "Base+P";
        case Base_ML:
            return "Base+ML";
        case Base_MP:
            return "Base+MP";
        case Base_LP:
            return "Base+LP";
        case Base_MLP:
            return "Base+MLP";
    }
    throw "Invalid game type";
}


//Type of bug

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

//Parsing

BugType parseBugType(string s);


enum class StrategyName {
    RANDOM = 0,
    MINIMAX = 1,
    DRL = 2
};

#endif