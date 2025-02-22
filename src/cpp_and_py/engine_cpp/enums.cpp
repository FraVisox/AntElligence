#include "enums.h"

/**
 * \brief Converts a PlayerColor enum to a string.
 *
 * \param c PlayerColor enum.
 * \return "b" if the color is BLACK, "w" if the color is WHITE.
 * \throws string If the color is invalid.
 */
string ColorToString(PlayerColor c){
    if(c==BLACK) return "b";
    if(c==WHITE) return "w";
    throw "Invalid color";
}

/**
 * \brief Parses a color string to a PlayerColor enum.
 *
 * \param s Color string. Must be either "b" for black or "w" for white.
 * \return Parsed PlayerColor enum.
 * \throws string If the color string is invalid.
 */
PlayerColor parseColor(string s){
    if(s=="b") return PlayerColor::BLACK;
    if(s=="w") return PlayerColor::WHITE;
    throw "Invalid color for string "+s;
}

/**
 * \brief Converts a GameState enum to a string.
 *
 * \param gs GameState enum.
 * \return A string representation of the GameState, as follows:
 * - STARTED and NOT_STARTED: "NotStarted"
 * - IN_PROGRESS: "InProgress"
 * - DRAW: "Draw"
 * - WHITE_WIN: "WhiteWins"
 * - BLACK_WIN: "BlackWins"
 * \throws string If the GameState is invalid.
 */
string GameStateToString(GameState gs){
    switch (gs)
    {
        case STARTED:
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
 * \brief Parses a GameState string to a GameState enum.
 *
 * \param s GameState string. Must be one of the following:
 * - "NotStarted" for the NOT_STARTED GameState
 * - "InProgress" for the IN_PROGRESS GameState
 * - "Draw" for the DRAW GameState
 * - "WhiteWins" for the WHITE_WIN GameState
 * - "BlackWins" for the BLACK_WIN GameState
 * \return Parsed GameState enum.
 * \throws string If the GameState string is invalid.
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
 * \brief Converts a GameType enum to a string representation.
 *
 * \param gt GameType enum.
 * \return A string representation of the GameType, as follows:
 * - Base: "Base"
 * - Base_L: "Base+L"
 * - Base_M: "Base+M"
 * - Base_P: "Base+P"
 * - Base_ML: "Base+ML"
 * - Base_MP: "Base+MP"
 * - Base_LP: "Base+LP"
 * - Base_MLP: "Base+MLP"
 * \throws string If the GameType is invalid.
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
 * \brief Parses a GameType string to a GameType enum.
 *
 * \param s GameType string. Must be one of the following:
 * - "Base" for the Base GameType
 * - "Base+L" for the Base_L GameType
 * - "Base+M" for the Base_M GameType
 * - "Base+P" for the Base_P GameType
 * - "Base+ML" for the Base_ML GameType
 * - "Base+MP" for the Base_MP GameType
 * - "Base+LP" for the Base_LP GameType
 * - "Base+MLP" for the Base_MLP GameType
 * \return Parsed GameType enum.
 * \throws string If the GameType string is invalid.
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
 * \brief Converts a BugType enum to a string.
 *
 * The string is a single character:
 * - Q for QUEEN
 * - S for SPIDER
 * - B for BEETLE
 * - G for GRASSHOPPER
 * - A for SOLDIER_ANT
 * - M for MOSQUITO
 * - L for LADYBUG
 * - P for PILLBUG
 *
 * \param t BugType enum.
 * \return A string representation of the BugType enum.
 */
string BugTypeToString(BugType t){
    int p=t;
    return string{_bug_string[p]};
}

/**
 * \brief Parses a BugType string to a BugType enum.
 *
 * The string is a single character:
 * - Q for QUEEN
 * - S for SPIDER
 * - B for BEETLE
 * - G for GRASSHOPPER
 * - A for SOLDIER_ANT
 * - M for MOSQUITO
 * - L for LADYBUG
 * - P for PILLBUG
 *
 * \param s BugType string.
 * \return Parsed BugType enum.
 * \throws string If the BugType string is invalid.
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