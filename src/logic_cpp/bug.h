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

string BugString(BugType t){
    int p=t;
    return string{_bug_string[p]};
}

enum BugType StringBug(string s){
    if(s=="Q") return BugType::QUEEN;
    if(s=="S") return BugType::SPIDER;
    if(s=="B") return BugType::BEETLE;
    if(s=="G") return BugType::GRASSHOPPER;
    if(s=="A") return BugType::SOLDIER_ANT;
    if(s=="M") return BugType::MOSQUITO;
    if(s=="L") return BugType::LADYBUG;
    if(s=="P") return BugType::PILLBUG;
    throw "Not valid bug type";
}


#endif