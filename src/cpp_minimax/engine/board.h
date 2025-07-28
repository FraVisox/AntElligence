#ifndef BOARD_H
#define BOARD_H

#define MAX_TURN_SIZE 1000
#include <bitset>
#include "gameboard.h"
#include "action.h"
#include <queue>
#include <string.h>
#include <unordered_map>
#include <string>
#include <cmath>
#include <algorithm>

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

struct MetricWeights {
    std::unordered_map<std::string, double> weights;

    double Get(BugType type, const std::string& key) const {
        std::string composed = std::to_string((int)type) + "_" + key;
        auto it = weights.find(composed);
        return (it != weights.end()) ? it->second : 0.0;
    }
    
    void Set(BugType type, const std::string& key, double value) {
        std::string composed = std::to_string((int)type) + "_" + key;
        weights[composed] = value;
    }
};

struct BoardMetrics {
    std::unordered_map<std::string, BugMetrics> pieceMetrics;
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

    void copy(Board&);


    //Current status
    PlayerColor currentColor();
    int currentPlayerTurn();
    bool placedQueen();
    
    void addPieceHand(pieceT p);

    // update the state
    void applayAction(actionT a);


    // compute possible moves
    void ComputePossibleMoves();


    GameState getGameState();
    
    int currentTurn;
    
    // describe the status
    gameboard G;
    bitset<32> inHandPiece;
    pieceT prevMoved[2];

    string toString();
    actionT suggestInitialMove();
    bitset<285> simple_hash(PlayerColor color);
    uint64_t getHash();

    int getScore(PlayerColor color);
    int getScoreBug(pieceT p);
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
    MetricWeights initializeDefaultWeights();
    std::string pieceToString(pieceT piece);
    BugMetrics calculatePieceMetrics(pieceT piece);
    BoardMetrics calculateBoardMetrics();
    double calculateEarlyGamePositionalScore();
    double calculateLateGameThreatScore();

    // Advanced evaluation function
    double evaluateAdvanced(PlayerColor playerColor);


    // returns the actions
    actionT resAction[MAX_ACTIONS_SIZE];
    int numAction;
    bitset<285> confHistory[MAX_TURN_SIZE+2];

    private:
    
    // auxiliary for pillbug
    actionT pillbugMoves[20];
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