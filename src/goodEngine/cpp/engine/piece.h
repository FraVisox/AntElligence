#ifndef PIECE_H
#define PIECE_H
#include <string>
#include "enums.h"


typedef char pieceT;

extern const pieceT INVALID_PIECE;
pieceT extractPiece(string s);
PlayerColor col(pieceT);
BugType kind(pieceT);
int numInc(pieceT);
pieceT buildPiece(enum BugType k,PlayerColor c,int numIncr=0);
string PiecetoString(pieceT p);
pieceT getCandidateForKind(BugType t,PlayerColor col);

extern pieceT simulatedPiece;
extern bool isSimulatingPiece;


#endif