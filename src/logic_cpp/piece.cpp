#include <string>
#include "piece.h"
#include <iostream>
using namespace std;



piece::piece(BugType k,PlayerColor c,int n){
    kind=k;
    col=c;
    numIncr=n;
}

piece::piece(string s){
    col=StringColor(s.substr(0,1));
    kind=StringBug(s.substr(1,1));
    if(s.size()==3){
        numIncr=stoi(s.substr(2,1));
    }else{
        numIncr=0;
    }
}


piece::piece(int eqVal){
    col=(PlayerColor)(eqVal%2);
    kind=(BugType)((eqVal%14)/2);
    numIncr=eqVal/14;
}

string piece::toString() const{
    return ColorString(col)+BugString(kind)+(numIncr==0?(""):(""+numIncr));
}

int piece::toVal() const{
    return col+kind*2+numIncr*14;
}

bool operator==(const piece& p1,const piece &p){
    return p1.toVal()==p.toVal();
}

bool operator!=(const piece& p1,const piece &p){
    return (p1.toVal()!=p.toVal());
}

