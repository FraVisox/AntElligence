#include "enums.h"

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

char _bug_string[]="QSBGAMLP";


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


int opposite(int x){
    if(x<6)
        return (x+3)%6;
    return 13-x;
}



string GameStateToString(GameState gs){
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

string ColorToCompleteString(PlayerColor c){
    if (c == PlayerColor::WHITE) return "White";
    return "Black";
}