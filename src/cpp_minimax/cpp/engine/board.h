#ifndef BOARD_H
#define BOARD_H

#define MAX_TURN_SIZE 300
#include <bitset>
#include "gameboard.h"
#include "action.h"
#include <queue>
#include <string.h>


//extern uint8_t isValidMoveBitmask[1575];
//extern actionT associatedAction[1575];
class Board {
    public:


    Board();
    Board(GameType);

    void copy(Board&);


    //Current status
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();
    
    void addPieceHand(pieceT p);

    // update the state
    void applayAction(actionT a);

    int getScore(PlayerColor color);

    
    // compute possible moves
    void ComputePossibleMoves();


    GameState getGameState();

    string toString();
    
    int currentTurn;
    
    // describe the status
    gameboard G;
    bitset<32> inHandPiece;
    pieceT prevMoved[2];

    // returns the actions
    actionT resAction[MAX_ACTIONS_SIZE];
    int numAction;
    bitset<308> confHistory[MAX_TURN_SIZE+2];

    private:
    
    // auxiliary for pillbug
    actionT pillbugMoves[20];
    int pillbugTotMoves;

    // auxiliary for Ants moves
    bitset<1024> inQueue;
    
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




    int getScore(PlayerColor color);
    int getScoreBug(pieceT p);
    int countSurrounding(pieceT p);
    int isPinned(pieceT bug);
    int isCovered(pieceT bug);
    int friendlyNeighbour(pieceT bug);
    int enemyNeighbour(pieceT bug);
};


#endif