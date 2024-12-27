#include <bits/stdc++.h>
#include "position.h"
#include "piece.h"
using namespace std;



piece::piece(enum BugType k,PlayerColor c,int n=0){
    kind=k;
    col=c;
    numIncr=n;
    name=ColorString(c)+BugString(k)+(n==0?(""):(""+n));
}
piece::piece(string s){
    this->name=s;
    col=StringColor(s.substr(0,1));
    kind=StringBug(s.substr(1,1));
    if(s.size()==3){
        numIncr=stoi(s.substr(2,1));
    }else{
        numIncr=0;
    }
}

   

bool operator==(const piece& p1,const piece &p){
    return p1.name==p.name;
}

bool operator!=(const piece& p1,const piece &p){
    return (p1.name!=p.name);
}

