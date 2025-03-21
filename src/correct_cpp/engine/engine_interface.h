#ifndef ENGINE_INTERFACE_H
#define ENGINE_INTERFACE_H

#include "board.h"
#include <regex>

extern Board b;
int startGame(char* s);

int getTurn();

double oracleEval();

int playMove(char* m);

static char MOVES_BUFFER[10000];

char* validMoves();

static char BOARD_BUFFER[100000];

const char* getBoard();
void undoB(int amount);

#endif