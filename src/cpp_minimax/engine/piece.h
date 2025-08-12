#ifndef PIECE_H
#define PIECE_H
#include <string>
#include <cstdint>
#include "enums.h"


typedef uint16_t pieceT;

extern const pieceT INVALID_PIECE;
pieceT extractPiece(string s);
const PlayerColor col(const pieceT);
const BugType kind(const pieceT&);
int numInc(pieceT);
pieceT buildPiece(enum BugType k,PlayerColor c,int numIncr=0);
string PiecetoString(pieceT p);
pieceT getCandidateForKind(BugType t,PlayerColor col);

pieceT decodeBug(char* c);
extern pieceT simulatedPiece;
extern bool isSimulatingPiece;


#endif