#include <string>
using namespace std;

#ifndef BUG_H
#define BUG_H

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
string BugToString(BugType t){
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
enum BugType StringToBug(string s){
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


#endif