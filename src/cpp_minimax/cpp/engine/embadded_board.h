
#ifndef EBOARD_H
#define EBOARD_H
//#include "graph_board/graph_board.h"
#include "board.h"
class EBoard{
    public:
    EBoard(GameType);
    EBoard(EBoard*);
    Board board_exp;

    bool isUpdated=false;
    pieceT vectRapp[1033];
    //char graph_board[BOARDSIZE];
    void applyAction(actionT a);
    void getNextsActions(actionT* ris);
    int getState();

    int getScore(PlayerColor color);

    void updateVectRapp();
    int getTurn();
    //void checkConsistency();
};
#endif