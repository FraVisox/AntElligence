#include <string>
#include "piece.h"
using namespace std;
#include <iostream>
const pieceT INVALID_PIECE=0;

pieceT extractPiece(string s){
    if(s[0]=='-' || s[0]=='/' || s[0]=='\\') return extractPiece(s.substr(1));

    int l=s.length();

    if(s[l-1]=='-' || s[l-1]=='/'|| s[l-1]=='\\') return extractPiece(s.substr(0,l-1));
    PlayerColor col=parseColor(s.substr(0,1));
    BugType kind=parseBugType(s.substr(1,1));
    int numIncr;
    if(s.size()==3){
        numIncr=stoi(s.substr(2,1));
    } else{
        numIncr=0;
    }
    return buildPiece(kind,col,numIncr);
}

PlayerColor col(pieceT p){

    if(p>0 && p<=14) return PlayerColor::WHITE;
    if(p>14 && p<=28) return PlayerColor::BLACK;
    if(p==29 && isSimulatingPiece) return col(simulatedPiece);
    std::cerr<<"Error in ,piece in col"<<std::endl;
    throw "Not correct color";
}

const BugType PIECE_KIND_NUM[]={
    BugType::SPIDER,
    BugType::SPIDER,
    BugType::BEETLE,
    BugType::BEETLE,
    BugType::GRASSHOPPER,
    BugType::GRASSHOPPER,
    BugType::GRASSHOPPER,
    BugType::QUEEN,
    BugType::SOLDIER_ANT,
    BugType::SOLDIER_ANT,
    BugType::SOLDIER_ANT,
    BugType::MOSQUITO,
    BugType::LADYBUG,
    BugType::PILLBUG
};

BugType kind(pieceT p){
    if(p>0 && p<=28)
        return PIECE_KIND_NUM[(p-1)%14];
    std::cerr<<"Error in piece, in kind: asking for kind of "<<p<<std::endl;
    throw "Asking kind of a non-piece";
}

const int NUM_INC_PIECE[]={
    1,2,
    1,2,
    1,2,3,
    0,
    1,2,3,
    0,0,0
};


int numInc(pieceT p){
    return NUM_INC_PIECE[(p-1)%14];
}

pieceT getCandidateForKind(BugType t,PlayerColor col){
    int inc=0;
    if(col==PlayerColor::BLACK)
        inc=14;
    switch (t)
    {
    case BugType::SPIDER:      return inc+1; break;  // 1-2
    case BugType::BEETLE:      return inc+3; break;  //  3-4
    case BugType::GRASSHOPPER: return inc+5; break;  // 5-6-7
    case BugType::QUEEN:       return inc+8; break;  // 8
    case BugType::SOLDIER_ANT: return inc+9; break;  // 9-10-11;
    case BugType::MOSQUITO:    return inc+12; break;  // 12
    case BugType::LADYBUG:     return inc+13; break;  // 13
    case BugType::PILLBUG:     return inc+14; break;  // 14
    default:std::cerr<<"Error in piece, in getCandidate"<<std::endl;
     throw "NOT VALID BUG TYPE";
    }
}

pieceT buildPiece(enum BugType k,PlayerColor c,int numIncr){
    char r=1;
    if(c==PlayerColor::BLACK){
        r+=14;
    }
    
    switch (k) {
    case BugType::SPIDER:       r+=numIncr-1; break;  // 1-2
    case BugType::BEETLE:       r+=numIncr+1; break;  //  3-4
    case BugType::GRASSHOPPER:  r+=numIncr+3; break;  // 5-6-7
    case BugType::QUEEN:        r+=7;         break;  // 8
    case BugType::SOLDIER_ANT:  r+=numIncr+7; break;  // 9-10-11;
    case BugType::MOSQUITO:     r+=11;        break;  // 12
    case BugType::LADYBUG:      r+=12;        break;  // 13
    case BugType::PILLBUG:      r+=13;        break;  // 14
    default: std::cerr<<"Error in piece, in buildPiece"<<std::endl;
    throw "NOT A KIND";
    }
    return r;
} 


string PIECE_STRING_RAPP[]={
    "   ",  //0
    "wS1",  //1
    "wS2",  //2
    "wB1",  //3
    "wB2",  //4
    "wG1",  //5
    "wG2",  //6
    "wG3",  //7
    "wQ",   //8
    "wA1",  //9
    "wA2",  //10
    "wA3",  //11
    "wM",   //12
    "wL",   //13
    "wP",   //14
    "bS1",  //15
    "bS2",  //16
    "bB1",  //17
    "bB2",  //18
    "bG1",  //19
    "bG2",  //20
    "bG3",  //21
    "bQ",   //22
    "bA1",  //23
    "bA2",  //24
    "bA3",  //25
    "bM",   //26
    "bL",   //27
    "bP",   //28
};

string PiecetoString(pieceT p){
    return PIECE_STRING_RAPP[(int)p];

}
pieceT decodeBug(char* s){
    PlayerColor col=((s[0]=='w')?PlayerColor::WHITE : PlayerColor::BLACK);
    BugType bt;
    int numInc=0;
    switch (s[1] |96)
    {
    case 's': numInc=(s[2]-'0'); bt=BugType::SPIDER; break;
    case 'b': numInc=(s[2]-'0'); bt=BugType::BEETLE; break;
    case 'g': numInc=(s[2]-'0'); bt=BugType::GRASSHOPPER; break;
    case 'a': numInc=(s[2]-'0'); bt=BugType::SOLDIER_ANT; break;
    case 'q': numInc=0; bt=BugType::QUEEN; break;
    case 'm': numInc=0; bt=BugType::MOSQUITO; break;
    case 'l': numInc=0; bt=BugType::LADYBUG; break;
    case 'p': numInc=0; bt=BugType::PILLBUG; break;
    default: std::cerr<<"Error in piece, in decodeBug"<<std::endl;
    throw "Not a valid bug";
    }
    return buildPiece(bt,col,numInc);
}





pieceT simulatedPiece;
bool isSimulatingPiece=false;