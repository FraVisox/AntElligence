
#ifndef EBOARD_H
#define EBOARD_H
//#include "graph_board/graph_board.h"
#include "engine/board.h"
class EBoard{
    public:
    EBoard(GameType);
    EBoard(EBoard*);
    Board board_exp;
    //char graph_board[BOARDSIZE];
    void applyAction(actionT a);
    void getNextsActions(actionT* ris);
    int getState();
    //void checkConsistency();
};
#endif