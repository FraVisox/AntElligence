#ifndef BOARD_H
#define BOARD_H
#include <bitset>
#include "gameboard.h"
#include "action.h"
#include <queue>
#include <string.h>

class Board {
    public:

    //Properties of this game
    int currentTurn;
    GameState state; //InProgress, Draw, WhiteWins, BlackWins, NotStarted
    GameType type; //Base, BASE+MLP, ...
    
    //Moves made




    //Gameboard and pieces
    gameboard G = gameboard();
    //vector<pieceT> placedBug = vector<pieceT>();
    bitset<32> inHandPiece;
    actionT resAction[MAX_ACTIONS_SIZE];
    int numAction;
    pieceT prevMoved[2];
    //Initialization
    actionT pillbugMoves[20];
    int pillbugTotMoves;

    position quePM[128];
    bitset<SIZE_BOARD*SIZE_BOARD> inQueue;

    Board();

    void copy(Board&);

    int getScore(PlayerColor color);
    int getScoreBug(pieceT p);
    int countSurrounding(pieceT p);

    //Current status
    string toString();
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();


    bitset<1024> movesAnt;
    //Make movement
    //ReturnMessage executeAction(string s);
    //ReturnMessage checkWin();
    //bool checkSurrounding(const pieceT &p);
    void applayAction(actionT a);

    void addPieceHand(pieceT p);

    void ComputePossibleMoves();
    void possibleMovesBug(pieceT b);
    void possibleMoves_Queen(pieceT bug);
    void possibleMoves_Beetle(pieceT bug);
    void possibleMoves_Grasshopper(pieceT bug);
    void possibleMoves_SoldierAnt(pieceT bugs);
    void possibleMoves_Spider(pieceT bugs);
    void possibleMoves_Pillbug(pieceT bug);
    void possibleMoves_Mosquito(pieceT bug);
    void possibleMoves_Ladybug(pieceT bug);

    int isPinned(pieceT bug);
    int isCovered(pieceT bug);
    int friendlyNeighbour(pieceT bug);
    int enemyNeighbour(pieceT bug);
    friend Board buildBoardFromGraph(); 
};


#endif