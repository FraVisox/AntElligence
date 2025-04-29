

#include "embadded_board.h"
#ifndef INTERFACE_H
#define INTERFACE_H
EBoard* base_state(int gt);
EBoard* copyBoard(EBoard* b);
void next_state(EBoard* state,actionT action);

void getActions(EBoard* state,int64_t* actions);
int checkStatus(EBoard* board);
char* toVectorNear(EBoard* b);
void PrintBoard(EBoard* b);
char* actionToString(actionT a);
double boardEval(EBoard* b, double w[]);
void delBoard(EBoard* b);

#endif