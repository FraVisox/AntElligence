#ifndef BUILD_FROM_GRAPH_H
#define BUILD_FROM_GRAPH_H
#include "engine/board.h"
#include "graph_board.h"

void DFSPlace(boardT board, gameboard& gb, position pos, int n);
gameboard buildGameBoardFromGraph(boardT board);
piece NumToPiece(int n);
Board buildBoardFromGraph(boardT board);
actionT PieceToNum(piece p);
#endif