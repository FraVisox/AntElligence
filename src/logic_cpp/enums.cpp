#include <string>
#include "bug.h"
using namespace std;

enum PlayerColor{
    WHITE=0,
    BLACK=1
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

