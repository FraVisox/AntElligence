#include "../engine/board.h"
#include "graph_board.h"

void UpdateState(boardT currentBoard, actionT action);

void UpdateBoardE(Board &b, actionT act);

void UpdateGameboard(gameboard &g, actionT act);

void  getActionsWithImplicitTransiction(boardT state,Board& b, actionT* ris);
