#ifndef ENUMS_H
#define ENUMS_H

#include <string>
using namespace std;
typedef char* boardT;
typedef int64_t actionT;

//Return message

enum ReturnMessage{ //TODO: add more errors
    OK = 0,
    ERROR = 1,
    GAME_OVER_DRAW = 2,
    GAME_OVER_WHITE_WINS = 3,
    GAME_OVER_BLACK_WINS = 4,
    INVALID_ARGUMENT = 5,
    INVALID_GAME_NOT_STARTED = 6,
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
    NOT_STARTED=1,
    STARTED=2,
    IN_PROGRESS=3,
    DRAW=4,
    WHITE_WIN=5,
    BLACK_WIN=6
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


//Engine enums
// Enum for commands
enum class Command {
    INFO,
    HELP,
    OPTIONS,
    NEWGAME,
    VALIDMOVES,
    BESTMOVE,
    PLAY,
    PASS,
    UNDO,
    EXIT,
    GET,
    SET,
    UNKNOWN
};

enum class StrategyName {
    RANDOM = 0,
    MINIMAX = 1,
    DRL = 2
};

// Convert string to Command enum
Command stringToCommand(const std::string& cmd);
std::string optionToString(StrategyName brain);
StrategyName stringToStrategyName(const std::string& brain);
#endif