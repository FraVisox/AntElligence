#ifndef ENUMS_H
#define ENUMS_H

#include <string>
using namespace std;

//Return message

enum ReturnMessage{ //TODO: add more errors
    OK = 0,
    ERROR = 1,
    GAME_OVER_DRAW = 2,
    GAME_OVER_WHITE_WINS = 3,
    GAME_OVER_BLACK_WINS = 4
};


//Color of the player

enum PlayerColor{
    WHITE=0,
    BLACK=1
};

//Parsing

string ColorToString(PlayerColor c);
PlayerColor parseColor(string s);

//Game state

enum GameState{
    NOT_STARTED,
    STARTED,
    IN_PROGRESS,
    DRAW,
    WHITE_WIN,
    BLACK_WIN
};

//Parsing

string GameStateToString(GameState gs);
GameState parseState(string s);

//Game type

enum GameType{
    Base,
    Base_M,
    Base_L,
    Base_P,
    Base_ML,
    Base_MP,
    Base_LP,
    Base_MLP
};

//Parsing

string GameTypeToString(GameType gt);
GameType parseGameType(string s);

//Type of bug

enum BugType{
    QUEEN=0,
    SPIDER=1,
    BEETLE=2,
    GRASSHOPPER=3,
    SOLDIER_ANT=4,
    MOSQUITO=5,
    LADYBUG=6,
    PILLBUG=7
};

//Parsing

string BugTypeToString(BugType t);
BugType parseBugType(string s);


#endif