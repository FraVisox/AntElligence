#ifndef BOARD_H
#define BOARD_H

#define MAX_TURN_SIZE 205
#include <bitset>
#include "gameboard.h"
#include "action.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include "weights.h"

//extern uint8_t isValidMoveBitmask[1575];
//extern actionT associatedAction[1575];
class Board {
    public:


    Board();
    Board(GameType);
    Board(const Board&,const actionT );


    bool hasUpdate;
    bool stateCached=false;
    GameState cache_state;
    //Current status
    PlayerColor currentColor() const;
    int currentPlayerTurn();
    bool placedQueen();
    
    void addPieceHand(pieceT p);

    // update the state
    void applayAction(actionT a);
    void undoAction();

    // compute possible moves
    void ComputePossibleMoves();


    GameState getGameState();
    
    int currentTurn;
    positionT prevPos[MAX_TURN_SIZE];
    pieceT   prevBug[MAX_TURN_SIZE];
    pieceT hisPM1[MAX_TURN_SIZE];
    pieceT hisPM2[MAX_TURN_SIZE];
    // describe the status
    gameboard G;
    bitset<32> inHandPiece;
    uint32_t prevMoved[2];

    string toString();
    actionT suggestInitialMove();
    size_t getHash() const;

    double getScoreBug(pieceT p);
    int isPinned(pieceT bug);
    int isCovered(pieceT bug);
    int friendlyNeighbour(pieceT bug);
    int enemyNeighbour(pieceT bug);

    short idxStartActions[32];
    // returns the actions
    actionT resAction[MAX_ACTIONS_SIZE];
    int numAction=0;
    //bitset<285> confHistory[MAX_TURN_SIZE+2];
    void printBoard();
    
    // auxiliary for pillbug
    actionT pillbugMoves[24];
    int pillbugTotMoves;

    // auxiliary for Ants moves
    BoardBitSet inQueue;
    
    void possibleMovesBug(pieceT b);
    void possibleMoves_Queen(pieceT bug);
    void possibleMoves_Beetle(pieceT bug);
    void possibleMoves_Grasshopper(pieceT bug);
    void possibleMoves_SoldierAnt(pieceT bugs);
    void possibleMoves_Spider(pieceT bugs);
    void possibleMoves_Pillbug(pieceT bug);
    void possibleMoves_Mosquito(pieceT bug);
    void possibleMoves_Ladybug(pieceT bug);
    void computePillbugMovinPieces();

    void controlValidirt();
};


#endif