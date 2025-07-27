// Better than what Mzinga uses
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>

// --- Enums and Structs ---

enum Color { WHITE, BLACK };
enum BugType { ANT, HOPPER, BEETLE, SPIDER, QUEEN, MOSQUITO, LADYBUG, PILLBUG };

struct Coord {
    int x, y;
    int distance_to(const Coord& other) const {
        return std::max({ std::abs(x - other.x), std::abs(y - other.y), std::abs((x + y) - (other.x + other.y)) });
    }
};

struct Bug {
    std::string name;
    BugType type;
    Color color;
    Coord position;
};

struct BugMetrics {
    bool InPlay = false;
    bool IsPinned = false;
    bool IsCovered = false;
    bool IsOnTop = false;
    bool AdjacentToAntOrBeetle = false;
    bool NearOwnQueen = false;
    bool CanReachQueenHole = false;

    int NoisyMoves = 0;
    int QuietMoves = 0;
    int FriendlyNeighbors = 0;
    int EnemyNeighbors = 0;
};

struct MetricWeights {
    std::unordered_map<std::string, double> weights;

    double Get(BugType type, const std::string& key) const {
        std::string composed = std::to_string(type) + "_" + key;
        auto it = weights.find(composed);
        return (it != weights.end()) ? it->second : 0.0;
    }
};

// --- Board Summary ---

struct BoardMetrics {
    std::unordered_map<std::string, BugMetrics> pieceMetrics;
    int PiecesInPlay = 0;
    int PiecesInHand = 0;

    int MyQueenSurround = 0;
    int TheirQueenSurround = 0;

    int MyQueenEscape = 0;
    int TheirQueenEscape = 0;

    int MyPinCount = 0;
    int TheirPinCount = 0;

    bool CanWinThisTurn = false;
    bool OpponentCanWinNextTurn = false;

    bool EnemyQueenPinned = false;
    bool MyQueenPinned = false;

    bool CanPlaceToWin = false;
    bool OpponentCanPlaceToWin = false;
};

// --- Evaluation Function ---

double CalculateEarlyGamePositionalScore() {
    return 50.0; // Stub — you can replace this with early game logic
}

double CalculateLateGameThreatScore() {
    return 150.0; // Stub — replace with real late game score
}

double evaluate_board(const std::vector<Bug>& allPieces, const BoardMetrics& boardMetrics, const MetricWeights& weights, Color playerColor) {
    double score = 0.0;

    for (const Bug& piece : allPieces) {
        const auto& m = boardMetrics.pieceMetrics.at(piece.name);
        int sign = (piece.color == playerColor) ? +1 : -1;
        BugType type = piece.type;

        score += sign * weights.Get(type, "InPlayWeight")           * m.InPlay;
        score += sign * weights.Get(type, "IsPinnedWeight")         * m.IsPinned;
        score += sign * weights.Get(type, "IsCoveredWeight")        * m.IsCovered;
        score += sign * weights.Get(type, "IsOnTopWeight")          * m.IsOnTop;

        score += sign * weights.Get(type, "NoisyMoveWeight")        * std::log1p(m.NoisyMoves);
        score += sign * weights.Get(type, "QuietMoveWeight")        * std::sqrt(m.QuietMoves);

        score += sign * weights.Get(type, "FriendlyNeighborWeight") * m.FriendlyNeighbors;
        score += sign * weights.Get(type, "EnemyNeighborWeight")    * m.EnemyNeighbors;

        if (type == MOSQUITO && m.AdjacentToAntOrBeetle)
            score += sign * weights.Get(type, "MosquitoPowerBonus");

        if (type == PILLBUG && m.NearOwnQueen)
            score += sign * weights.Get(type, "DefensivePillbugBonus");

        if (type == LADYBUG && m.CanReachQueenHole)
            score += sign * weights.Get(type, "QueenDiveBonus");
    }

    // Global tactical features
    score += 200.0 * (boardMetrics.TheirQueenSurround - boardMetrics.MyQueenSurround);
    score += 15.0 * (boardMetrics.TheirQueenEscape - boardMetrics.MyQueenEscape);

    if (boardMetrics.CanWinThisTurn)        return  1e6;
    if (boardMetrics.OpponentCanWinNextTurn) return -1e6;

    if (boardMetrics.EnemyQueenPinned) score += 150.0;
    if (boardMetrics.MyQueenPinned)    score -= 150.0;

    score += 10.0 * boardMetrics.MyPinCount;
    score -= 10.0 * boardMetrics.TheirPinCount;

    if (boardMetrics.CanPlaceToWin)         score += 500.0;
    if (boardMetrics.OpponentCanPlaceToWin) score -= 500.0;

    // Early vs late game blend
    int inHand = boardMetrics.PiecesInHand;
    int inPlay = boardMetrics.PiecesInPlay;
    double earlyRatio = inHand / double(inHand + inPlay + 1); // +1 avoids div by zero
    double lateRatio = 1.0 - earlyRatio;

    double earlyScore = CalculateEarlyGamePositionalScore();
    double lateScore  = CalculateLateGameThreatScore();

    score += (earlyRatio * earlyScore) + (lateRatio * lateScore);

    return score;
}

// --- Example Usage ---

int main() {
    // Define example bugs
    std::vector<Bug> pieces = {
        { "wA1", ANT, WHITE, {0, 0} },
        { "bQ", QUEEN, BLACK, {1, 0} },
        { "wM", MOSQUITO, WHITE, {-1, 1} }
    };

    // Set up dummy metrics
    BoardMetrics metrics;
    metrics.PiecesInHand = 5;
    metrics.PiecesInPlay = 6;
    metrics.MyQueenSurround = 3;
    metrics.TheirQueenSurround = 5;
    metrics.MyQueenEscape = 1;
    metrics.TheirQueenEscape = 0;
    metrics.CanWinThisTurn = false;
    metrics.OpponentCanWinNextTurn = false;
    metrics.EnemyQueenPinned = true;
    metrics.MyQueenPinned = false;

    metrics.pieceMetrics["wA1"] = {true, false, false, true, false, true, false, 2, 1, 2, 1};
    metrics.pieceMetrics["bQ"]  = {true, false, false, false, false, false, false, 1, 0, 1, 2};
    metrics.pieceMetrics["wM"]  = {true, false, false, false, true, false, false, 3, 2, 1, 1};

    // Dummy weights
    MetricWeights weights;
    weights.weights["0_InPlayWeight"] = 50;
    weights.weights["0_NoisyMoveWeight"] = 20;
    weights.weights["5_MosquitoPowerBonus"] = 30;
    weights.weights["6_QueenDiveBonus"] = 25;
    weights.weights["7_DefensivePillbugBonus"] = 40;

    double result = evaluate_board(pieces, metrics, weights, WHITE);
    std::cout << "Board evaluation (White): " << result << "\n";

    return 0;
}
