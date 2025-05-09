#ifndef ENUMS_H
#define ENUMS_H

#include <string>
using namespace std;
typedef char* boardT;
typedef uint64_t actionT;
const  int MAX_ACTIONS_SIZE=256;
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
    Base=0,
    Base_M=1,
    Base_L=2,
    Base_P=3,
    Base_ML=4,
    Base_MP=5,
    Base_LP=6,
    Base_MLP=7
};

//Parsing

string GameTypeToString(GameType gt);
GameType parseGameType(string s);

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