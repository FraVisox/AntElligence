#include <algorithm>
#include <iostream>
using namespace std;

#include "../engine/board.h"
#define WHITENUM 0
#define INFOBOARD_SIZE 4
#define BOARDSIZE 284
#define INFOPOS 0
#define TURNPOS 2



int opposite(int x);
int getStartingPointBug(int bugNumber );

bool isPlaced(boardT board, int n);
bool isPresent(boardT board, int n);
bool isInHand(boardT board, int n);

void placePieceG(boardT board, int n);
void addPieceInHand(boardT board, int n);


void printBG(gameboard &g);
int currentTurn(boardT b);
void setTurn(boardT b,int n);
int checkWin(boardT b);
void findNextState(boardT currentBoard, boardT nextBoard,actionT act);
void getActionsWithExplicitTransiction(char*,actionT*);

void printBoardFancy(boardT board);
void printActionFancy(actionT a);


void checkBoardCoherent(boardT s);