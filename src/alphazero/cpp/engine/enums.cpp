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