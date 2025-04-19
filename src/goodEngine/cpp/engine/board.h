#ifndef BOARD_H
#define BOARD_H

#include "gameboard.h"
#include "action.h"
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <string.h>
#include <sstream>

class Board {
    public:

    //Properties of this game
    int currentTurn;
    GameState state; //InProgress, Draw, WhiteWins, BlackWins, NotStarted
    GameType type; //Base, BASE+MLP, ...
    
    //Moves made

    //Gameboard and pieces
    gameboard G = gameboard();
    vector<pieceT> placedBug = vector<pieceT>();
    unordered_set<pieceT> inHandPiece = unordered_set<pieceT>();
    bool isPlacedWQ=false;
    bool isPlacedBQ=false;

    //Initialization

    Board();

    void reset();

    int getScore(PlayerColor color);
    int getScoreBug(pieceT p);
    int countSurrounding(pieceT p);

    //Current status
    string toString();
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();

    //Make movement
    ReturnMessage executeAction(string s);
    ReturnMessage checkWin();
    bool checkSurrounding(const pieceT &p);


    void addPieceHand(pieceT p);

    vector<action> possibleMoves();
    void possibleMovesBug(pieceT b, vector<action> &res);
    void possibleMoves_Queen(pieceT bug,vector<action> &res);
    void possibleMoves_Beetle(pieceT bug,vector<action> &res);
    void possibleMoves_Grasshopper(pieceT bug,vector<action> &res);
    void possibleMoves_SoldierAnt(pieceT bug, vector<action> & res);
    void possibleMoves_Spider(pieceT bug, vector<action> & res);
    void possibleMoves_Pillbug(pieceT bug, vector<action> &res);
    void possibleMoves_Mosquito(pieceT bug, vector<action> &res);
    void possibleMoves_Ladybug(pieceT bug,vector<action> &res);

    int isPinned(pieceT bug);
    int isInPlay(pieceT bug);
    int isCovered(pieceT bug);
    int friendlyNeighbour(pieceT bug);
    int enemyNeighbour(pieceT bug);
    friend Board buildBoardFromGraph(); 
};


#endif