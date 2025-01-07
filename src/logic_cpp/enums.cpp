#include <string>
#include "bug.h"
using namespace std;

enum PlayerColor{
    WHITE=0,
    BLACK=1
};

/**
 * \brief Converts a PlayerColor enum to a string.
 *
 * Given a PlayerColor, this function returns the corresponding string
 * representation, which is either "b" for black or "w" for white.
 *
 * \param c The PlayerColor to convert.
 * \return The string representation of the given PlayerColor.
 * \throws string "Invalid color" if c is neither BLACK nor WHITE.
 */
string ColorToString(PlayerColor c){
    if(c==BLACK) return "b";
    if(c==WHITE) return "w";
    throw "Invalid color";
}

/**
 * \brief Converts a string to a PlayerColor enum.
 *
 * Given a string, this function returns the corresponding PlayerColor enum
 * value, which is either BLACK or WHITE.
 *
 * \param s The string to convert.
 * \return The PlayerColor enum value of the given string.
 * \throws string "Invalid color for string "+s if s is neither "b" nor "w".
 */
PlayerColor StringToColor(string s){
    if(s=="b") return PlayerColor::BLACK;
    if(s=="w") return PlayerColor::WHITE;
    throw "Invalid color for string "+s;
}

enum GameState{
    NOT_STARTED,
    IN_PROGRESS,
    DRAW,
    WHITE_WIN,
    BLACK_WIN
};


/**
 * \brief Converts a string to a GameState enum.
 *
 * Given a string, this function returns the corresponding GameState enum
 * value.
 *
 * \param s The string to convert.
 * \return The GameState enum value of the given string.
 * \throws string "Not valid game state for string "+s if s is not one of the
 *   valid GameStateString values.
 */
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
    throw "Invalid game state for string " + s;
}

/**
 * \brief Converts a GameState enum to a string.
 *
 * Given a GameState enum, this function returns the corresponding GameStateString
 * value.
 *
 * \param gs The GameState enum to convert.
 * \return The GameStateString value of the given GameState enum.
 * \throws string "Invalid game state" if gs is not one of the valid GameState
 *   values.
 */
string GameStateToString(GameState gs){
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
    throw "Invalid game state";
}

