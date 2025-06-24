#ifndef INTERFACE_H
#define INTERFACE_H

#include "embadded_board.h"

extern uint8_t maskBuffer*;
extern actionT validMovesBuffer*;

actionT stringToAction(EBoard* b, char* str);
void* getMask(EBoard* p);
void* getAssociatedAction(EBoard* p);
EBoard* base_state(int gt);
EBoard* copyBoard(EBoard* b);
void next_state(EBoard* state, actionT action);
void getActions(EBoard* state, actionT* actions);
void updatePossiblemoves(EBoard* state);
int checkStatus(EBoard* board);
void* getStatusVector(EBoard* board);
void printBoard(EBoard* board);
void actionToString(actionT a, EBoard *board, char* risBug);
double boardEval(EBoard* b, double w[]);
void delBoard(EBoard* b);


// Forward declaration of the board structure


#endif  // INTERFACE_H
