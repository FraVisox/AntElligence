#ifndef BOARD_H
#define BOARD_H

#define MAX_TURN_SIZE 1000
#include <bitset>
#include "gameboard.h"
#include "action.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include "weights.h"

// =============================================================================
// ADVANCED EVALUATION STRUCTURES
// =============================================================================

struct BugMetrics {
    bool InPlay = false;
    bool IsPinned = false;
    bool IsCovered = false;
    bool IsOnTop = false;
    bool AdjacentToAntOrBeetle = false;
    bool NearOwnQueen = false;
    bool CanReachQueenHole = false;

    int NoisyMoves = 0;      // Moves that threaten or capture
    int QuietMoves = 0;      // Positional moves
    int FriendlyNeighbors = 0;
    int EnemyNeighbors = 0;
};



struct BoardMetrics {
    BugMetrics pieceMetrics[29];
    int PiecesInPlay = 0;
    int PiecesInHand = 0;

    int MyQueenSurround = 0;
    int TheirQueenSurround = 0;

    int MyQueenEscape = 0;      // Number of free spaces around my queen
    int TheirQueenEscape = 0;   // Number of free spaces around their queen

    int MyPinCount = 0;
    int TheirPinCount = 0;

    bool CanWinThisTurn = false;
    bool OpponentCanWinNextTurn = false;

    bool EnemyQueenPinned = false;
    bool MyQueenPinned = false;

    bool CanPlaceToWin = false;
    bool OpponentCanPlaceToWin = false;
};

//extern uint8_t isValidMoveBitmask[1575];
//extern actionT associatedAction[1575];
class Board {
    public:


    Board();
    Board(GameType);
    Board(bool, bool);

    void copy(Board&);

    bool hasUpdate;
    //Current status
    PlayerColor currentColor();
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
    bitset<285> simple_hash();
    uint64_t getHash();

    double getScore(PlayerColor color);
    double getScoreBug(pieceT p);
    int surround_score();
    int mobility_score();
    int isPinned(pieceT bug);
    int isCovered(pieceT bug);
    int friendlyNeighbour(pieceT bug);
    int enemyNeighbour(pieceT bug);

    // Helper methods
    pieceT getQueenForColor(PlayerColor color);
    pieceT getOpposingQueen();
    pieceT getMyQueen();
    bool bothQueensPlaced();
    bool isAdjacent(positionT pos1, positionT pos2);
    int getHexDistance(positionT pos1, positionT pos2);

    // Scoring methods  
    int countMovesNearOpposingQueen();
    int pin_score();
    int near_moves_score();
    int tempo_score();
    int control_score();
    int bug_utility_score();

    // Advanced evaluation helpers
    std::string pieceToString(pieceT piece);
    BugMetrics calculatePieceMetrics(pieceT piece);
    BoardMetrics calculateBoardMetrics();
    double calculateEarlyGamePositionalScore();
    double calculateLateGameThreatScore();

    // Advanced evaluation function
    double evaluateAdvanced(PlayerColor playerColor,int=0);

    short idxStartActions[32];
    // returns the actions
    actionT resAction[MAX_ACTIONS_SIZE];
    int numAction;
    //bitset<285> confHistory[MAX_TURN_SIZE+2];
    void printBoard();
    private:
    
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

};


#endif