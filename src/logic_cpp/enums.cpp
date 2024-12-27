#include <string>
using namespace std;


enum BugType{
    QUEEN,
    SPIDER,
    BEETLE,
    GRASSHOPPER,
    SOLDIER_ANT,
    MOSQUITO,
    LADYBUG,
    PILLBUG
};



string BugString(BugType t){
    switch (t)
    {
    case QUEEN:
        return "Q";
    case SPIDER:
        return "S";
    case BEETLE:
        return "B";
    case GRASSHOPPER:
        return "G";
    case SOLDIER_ANT:
        return "A";
    case MOSQUITO:
        return "M";
    case LADYBUG:
        return "L";
    case PILLBUG:
        return "P";
    }
    throw "Not valid type of bug";
}

enum BugType StringBug(string s){
    if(s=="Q")
        return BugType::QUEEN;
    if(s=="S")
        return BugType::SPIDER;
    if(s=="B")
        return BugType::BEETLE;
    if(s=="G")
        return BugType::GRASSHOPPER;
    if(s=="A")
        return BugType::SOLDIER_ANT;
    if(s=="M")
        return BugType::MOSQUITO;
    if(s=="L")
        return BugType::LADYBUG;
    if(s=="P")
        return BugType::PILLBUG;
    throw "Not valid bug type";
}


enum Direction{
    RIGHT=0,
    DOWN_RIGHT=1,
    DOWN_LEFT=2,
    LEFT=3,
    UP_LEFT=4,
    UP_RIGHT=5
};


enum PlayerColor{
    WHITE,
    BLACK
};

string ColorString(PlayerColor c){
    if(c==BLACK) return "b";
    if(c==WHITE) return "w";
    throw "Not valid color";
}

PlayerColor StringColor(string s){
    if(s=="b") return PlayerColor::BLACK;
    if(s=="w") return PlayerColor::WHITE;
    throw "Not valid color for string "+s;
}

enum GameState{
    NOT_STARTED,
    IN_PROGRESS,
    DRAW,
    WHITE_WIN,
    BLACK_WIN
};


GameState StringToState(string s){
    if(s=="NotStarted")
        return GameState::NOT_STARTED;
    if(s=="InProgress")
        return GameState::IN_PROGRESS;
    if(s=="Draw")
        return GameState::DRAW;
    if(s=="WhiteWins")
        return GameState::WHITE_WIN;
    if(s=="BlackWins")
        return GameState::BLACK_WIN;
    throw "Not valid game state for string " + s;
}

string GameStateString(GameState gs){
    switch (gs)
    {
    case NOT_STARTED:
        return "NotStarted";
    case IN_PROGRESS:
        return "InProgress";
    case DRAW:
        return "Draw";
    case WHITE_WIN:
        return "WhiteWins";
    case BLACK_WIN:
        return "BlackWins";
    }
    throw "Not valid game state";
}

