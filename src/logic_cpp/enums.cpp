#include "enums.h"

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
PlayerColor parseColor(string s){
    if(s=="b") return PlayerColor::BLACK;
    if(s=="w") return PlayerColor::WHITE;
    throw "Invalid color for string "+s;
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
GameState parseState(string s){
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
 * \brief Converts a GameType enum to a string.
 *
 * Given a GameType enum, this function returns the corresponding string
 * representation, which describes the base and any expansions included.
 *
 * \param gt The GameType enum to convert.
 * \return The string representation of the given GameType.
 * \throws string "Invalid game type" if gt is not one of the valid GameType
 *   values.
 */
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


/**
 * \brief Converts a string representation of a GameType to the corresponding
 *   enum.
 *
 * Given a string that represents a GameType, this function returns the
 *   corresponding GameType enum value.
 *
 * \param s The string representation of the GameType, which should be one of
 *   the valid GameType values.
 * \return The GameType enum value corresponding to the given string.
 * \throws string "Invalid game type for string " + s if s is not one of the
 *   valid GameType string values.
 */
GameType parseGameType(string s){
    if(s=="Base")
        return Base;
    if(s=="Base+L")
        return Base_L;
    if(s=="Base+M")
        return Base_M;
    if(s=="Base+P")
        return Base_P;
    if(s=="Base+ML")
        return Base_ML;
    if(s=="Base+MP")
        return Base_MP;
    if(s=="Base+LP")
        return Base_LP;
    if(s=="Base+MLP")
        return Base_MLP;
    throw "Invalid game type for string " + s;
}


char _bug_string[]="QSBGAMLP";

/**
 * \brief Converts a BugType to its corresponding string representation.
 *
 * Given a BugType enumeration value, this function returns the single
 * character string that represents the bug type.
 *
 * \param t The BugType to be converted to a string.
 * \return A string representing the BugType.
 */
string BugTypeToString(BugType t){
    int p=t;
    return string{_bug_string[p]};
}

/**
 * \brief Converts a string representation of a bug type to its enumeration value.
 *
 * Given a single character string representing a bug type, this function returns
 * the corresponding BugType enumeration value.
 *
 * \param s The string to be converted to a BugType.
 * \return A BugType enumeration value.
 * \throw string Not valid bug type. If the input string is not a valid bug type.
 */
enum BugType parseBugType(string s){
    if(s=="Q") return BugType::QUEEN;
    if(s=="S") return BugType::SPIDER;
    if(s=="B") return BugType::BEETLE;
    if(s=="G") return BugType::GRASSHOPPER;
    if(s=="A") return BugType::SOLDIER_ANT;
    if(s=="M") return BugType::MOSQUITO;
    if(s=="L") return BugType::LADYBUG;
    if(s=="P") return BugType::PILLBUG;
    throw "Invalid bug type";
}