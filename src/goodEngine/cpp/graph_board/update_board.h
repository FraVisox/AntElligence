#include "graph_board.h"
#include "../engine/board.h"


void UpdateState(boardT currentBoard, actionT action);

void UpdateBoardE(Board &b,boardT prev, actionT act);

void UpdateGameboard(gameboard &g, actionT act,boardT prev);

void  getActionsWithImplicitTransiction(boardT state,Board& b, actionT ris);