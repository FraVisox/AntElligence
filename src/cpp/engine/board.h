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
    vector<action> moves = vector<action>(); 
    vector<action> possibleMovesVector = vector<action>(); 

    //Gameboard and pieces
    gameboard G = gameboard();
    vector<piece> placedBug = vector<piece>();
    unordered_set<piece> inHandPiece = unordered_set<piece>();
    bool isPlacedWQ=false;
    bool isPlacedBQ=false;

    //Initialization

    Board();

    void reset();
    void copy(const Board& b);

    action suggestInitialMove();
    int getScore(PlayerColor color);
    int getScoreBug(piece p);
    int countSurrounding(piece p);

    //Current status
    string toString();
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();

    //Make movement
    ReturnMessage executeAction(string s);
    action validMove(string s);
    ReturnMessage checkWin();
    bool checkSurrounding(const piece &p);

    void undo(int amount);

    void addPieceHand(piece p);

    vector<action> possibleMoves();
    void possibleMovesBug(piece b, vector<action> &res);
    void possibleMoves_Queen(piece bug,vector<action> &res);
    void possibleMoves_Beetle(piece bug,vector<action> &res);
    void possibleMoves_Grasshopper(piece bug,vector<action> &res);
    void possibleMoves_SoldierAnt(piece bug, vector<action> & res);
    void possibleMoves_Spider(piece bug, vector<action> & res);
    void possibleMoves_Pillbug(piece bug, vector<action> &res);
    void possibleMoves_Mosquito(piece bug, vector<action> &res);
    void possibleMoves_Ladybug(piece bug,vector<action> &res);

    int isPinned(piece bug);
    int isInPlay(piece bug);
    int isCovered(piece bug);
    int friendlyNeighbour(piece bug);
    int enemyNeighbour(piece bug);
};


#endif