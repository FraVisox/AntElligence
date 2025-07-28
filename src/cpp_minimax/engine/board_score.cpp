#include "board.h"
#include <set>
#include <map>
#include <queue>

// =============================================================================
// HELPER METHODS
// =============================================================================

/**
 * Gets the queen piece ID for the specified color
 */
pieceT Board::getQueenForColor(PlayerColor color) {
    return (color == PlayerColor::WHITE) ? 8 : 22;
}

/**
 * Gets the opposing queen piece ID for the current player
 */
pieceT Board::getOpposingQueen() {
    return (currentColor() == PlayerColor::WHITE) ? 22 : 8;
}

/**
 * Gets the current player's queen piece ID
 */
pieceT Board::getMyQueen() {
    return (currentColor() == PlayerColor::WHITE) ? 8 : 22;
}

/**
 * Checks if both queens are placed on the board
 */
bool Board::bothQueensPlaced() {
    return G.isPlaced[8] && G.isPlaced[22];
}

/**
 * Helper function to check if two positions are adjacent on the hex board.
 */
bool Board::isAdjacent(positionT pos1, positionT pos2) {
    for (int dir = 0; dir < 6; dir++) {
        if (applayMove(pos1, dir) == pos2) {
            return true;
        }
    }
    return false;
}

/**
 * Calculate the hex distance between two positions using BFS.
 */
int Board::getHexDistance(positionT pos1, positionT pos2) {
    if (pos1 == pos2) {
        return 0;
    }
    
    std::queue<positionT> queue;
    std::set<positionT> visited;
    std::map<positionT, int> distance;
    
    queue.push(pos1);
    visited.insert(pos1);
    distance[pos1] = 0;
    
    while (!queue.empty()) {
        positionT current = queue.front();
        queue.pop();
        
        for (int dir = 0; dir < 6; dir++) {
            positionT neighbor = applayMove(current, dir);
            
            if (neighbor == pos2) {
                return distance[current] + 1;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                distance[neighbor] = distance[current] + 1;
                
                // Prevent infinite search
                if (distance[neighbor] < 10) {
                    queue.push(neighbor);
                }
            }
        }
    }
    
    return -1; // Should not happen in connected board
}

// =============================================================================
// PIECE ANALYSIS METHODS
// =============================================================================

/**
 * Count friendly neighbors around a piece
 */
int Board::friendlyNeighbour(pieceT bug) {
    if (!G.isPlaced[bug]) return 0;
    
    int count = 0;
    positionT bugPos = G.getPosition(bug);
    PlayerColor bugColor = col(bug);
    
    for (int dir = 0; dir < 6; dir++) {
        positionT adjPos = applayMove(bugPos, dir);
        if (G.isFree(adjPos) || col(G.topPiece(adjPos)) == bugColor) {
            count++;
        }
    }
    return count;
}

/**
 * Count enemy neighbors around a piece
 */
int Board::enemyNeighbour(pieceT bug) {
    if (!G.isPlaced[bug]) return 0;
    
    int count = 0;
    positionT bugPos = G.getPosition(bug);
    PlayerColor bugColor = col(bug);
    
    for (int dir = 0; dir < 6; dir++) {
        positionT adjPos = applayMove(bugPos, dir);
        if (!G.isFree(adjPos) && col(G.topPiece(adjPos)) != bugColor) {
            count++;
        }
    }
    return count;
}

/**
 * Check if a piece is pinned (cannot move without breaking hive)
 */
int Board::isPinned(pieceT bug) {
    if (!G.isPlaced[bug]) return 0;
    return !G.canPieceMove(bug, currentTurn) ? 1 : 0;
}

/**
 * Check if a piece is covered by another piece
 */
int Board::isCovered(pieceT bug) {
    if (!G.isPlaced[bug]) return 0;
    return !G.isTop(bug) ? 1 : 0;
}

// =============================================================================
// SCORING METHODS
// =============================================================================

/**
 * Score based on how surrounded each queen is
 * Positive score = opponent's queen more surrounded (good for us)
 * Negative score = our queen more surrounded (bad for us)
 */
int Board::surround_score() {
    if (!bothQueensPlaced()) {
        return 0;
    }
    
    const int PENALTY_PER_NEIGHBOR = 20;  // Cost per neighbor around our queen
    const int BONUS_PER_NEIGHBOR = 15;    // Bonus per neighbor around opponent's queen
    
    pieceT myQueen = getMyQueen();
    pieceT opponentQueen = getOpposingQueen();
    
    int score = 0;
    
    // Count neighbors around our queen (negative impact)
    positionT myQueenPos = G.getPosition(myQueen);
    for (int dir = 0; dir < 6; dir++) {
        positionT adjPos = applayMove(myQueenPos, dir);
        if (!G.isFree(adjPos)) {
            score -= PENALTY_PER_NEIGHBOR;
        }
    }
    
    // Count neighbors around opponent's queen (positive impact)
    positionT oppQueenPos = G.getPosition(opponentQueen);
    for (int dir = 0; dir < 6; dir++) {
        positionT adjPos = applayMove(oppQueenPos, dir);
        if (!G.isFree(adjPos)) {
            score += BONUS_PER_NEIGHBOR;
        }
    }
    
    return score;
}

/**
 * Score based on piece mobility (pieces that can move freely)
 */
int Board::mobility_score() {
    if (!bothQueensPlaced()) {
        return 0;
    }
    
    int score = 0;
    PlayerColor myColor = currentColor();
    
    // Mobility values for different piece types
    const int mobilityValues[8] = {
        0,  // QUEEN = 0
        3,  // SPIDER = 1
        6,  // BEETLE = 2
        8,  // GRASSHOPPER = 3
        10, // SOLDIER_ANT = 4
        8,  // MOSQUITO = 5
        5,  // LADYBUG = 6
        1   // PILLBUG = 7
    };
    
    for (pieceT p = 1; p <= 28; p++) {
        if (G.isPlaced[p] && !isPinned(p) && !isCovered(p)) {
            int pieceValue = mobilityValues[kind(p)];
            
            if (col(p) == myColor) {
                score += pieceValue;  // Our mobile pieces are good
            } else {
                score -= pieceValue;  // Opponent's mobile pieces are bad
            }
        }
    }
    
    return score;
}

/**
 * Score based on pieces positioned near the opponent's queen (tempo advantage)
 */
int Board::tempo_score() {
    if (!bothQueensPlaced()) {
        return 0;
    }
    
    const int TEMPO_BONUS = 6;
    const int MAX_TEMPO_DISTANCE = 2;
    
    int score = 0;
    PlayerColor myColor = currentColor();
    pieceT opponentQueen = getOpposingQueen();
    positionT oppQueenPos = G.getPosition(opponentQueen);
    
    for (pieceT p = 1; p <= 28; p++) {
        if (G.isPlaced[p] && col(p) == myColor && !isPinned(p) && !isCovered(p)) {
            positionT piecePos = G.getPosition(p);
            if (getHexDistance(piecePos, oppQueenPos) <= MAX_TEMPO_DISTANCE) {
                score += TEMPO_BONUS;
            }
        }
    }
    
    return score;
}

/**
 * Count how many valid moves lead to positions adjacent to the opposing queen
 */
int Board::countMovesNearOpposingQueen() {
    ComputePossibleMoves();
    
    pieceT opponentQueen = getOpposingQueen();
    if (!G.isPlaced[opponentQueen]) {
        return 0;
    }
    
    positionT queenPos = G.getPosition(opponentQueen);
    int count = 0;
    
    for (int i = 0; i < numAction; i++) {
        actionT action = resAction[i];
        
        if (action == 0) continue;  // Skip pass moves
        
        positionT destPos = action / 32;  // Extract destination position
        
        if (isAdjacent(destPos, queenPos)) {
            count++;
        }
    }
    
    return count;
}

/**
 * Score based on how many moves we have that threaten the opponent's queen
 */
int Board::near_moves_score() {
    return countMovesNearOpposingQueen() * 2;  // Each threatening move worth 2 points
}

/**
 * Score based on control around the opponent's queen
 */
int Board::control_score() {
    if (!bothQueensPlaced()) {
        return 0;
    }
    
    const int CONTROL_BONUS = 5;
    pieceT opponentQueen = getOpposingQueen();
    
    return CONTROL_BONUS * enemyNeighbour(opponentQueen);
}

/**
 * Score based on pinned pieces (immobilized pieces)
 */
int Board::pin_score() {
    const int PIN_VALUE = 10;
    int score = 0;
    PlayerColor myColor = currentColor();
    
    for (pieceT p = 1; p <= 28; p++) {
        if (G.isPlaced[p] && isPinned(p)) {
            BugType pieceType = kind(p);
            
            // Only count valuable pieces as significant when pinned
            if (pieceType == SOLDIER_ANT || pieceType == BEETLE || pieceType == GRASSHOPPER) {
                if (col(p) == myColor) {
                    score -= PIN_VALUE;  // Our pinned pieces are bad
                } else {
                    score += PIN_VALUE;  // Opponent's pinned pieces are good
                }
            }
        }
    }
    
    return score;
}

/**
 * Score based on strategic positioning of special pieces
 */
int Board::bug_utility_score() {
    if (!bothQueensPlaced()) {
        return 0;
    }
    
    int score = 0;
    positionT myQueenPos = G.getPosition(getMyQueen());
    positionT oppQueenPos = G.getPosition(getOpposingQueen());
    PlayerColor myColor = currentColor();
    
    for (pieceT p = 1; p <= 28; p++) {
        if (!G.isPlaced[p] || col(p) != myColor) continue;
        
        positionT piecePos = G.getPosition(p);
        
        switch (kind(p)) {
            case MOSQUITO:
                // Mosquito is more valuable when adjacent to powerful pieces
                for (int dir = 0; dir < 6; dir++) {
                    positionT adjPos = applayMove(piecePos, dir);
                    if (!G.isFree(adjPos)) {
                        pieceT adjPiece = G.topPiece(adjPos);
                        BugType adjType = kind(adjPiece);
                        if (adjType == BEETLE || adjType == SOLDIER_ANT) {
                            score += 5;
                            break;  // Only count once
                        }
                    }
                }
                break;
                
            case LADYBUG:
                // Ladybug is valuable when near our queen for defense
                if (getHexDistance(piecePos, myQueenPos) <= 2) {
                    score += 5;
                }
                break;
                
            case PILLBUG:
                // Pillbug is valuable near both queens
                if (getHexDistance(piecePos, myQueenPos) <= 2) {
                    score += 6;  // Defensive value
                }
                if (getHexDistance(piecePos, oppQueenPos) <= 2) {
                    score += 8;  // Offensive value
                }
                break;
                
            default:
                break;
        }
    }
    
    return score;
}

/**
 * Individual piece scoring based on position and mobility
 */
int Board::getScoreBug(pieceT p) {
    if (!G.isPlaced[p]) return 0;
    
    // Base values for different piece types
    const int kindValue[] = {100, 10, 70, 40, 90, 35, 60, 45};
    
    int baseValue = kindValue[kind(p)];
    
    if (isPinned(p) || isCovered(p)) {
        return -baseValue;  // Immobilized pieces are liabilities
    }
    
    // Factor in positioning: friendly neighbors good, enemy neighbors bad
    int positionModifier = friendlyNeighbour(p) - enemyNeighbour(p);
    
    return baseValue * positionModifier;
}

/**
 * Overall board evaluation from the perspective of the given color
 */
int Board::getScore(PlayerColor color) {
    /*
    // Temporarily switch perspective if needed
    PlayerColor originalColor = currentColor();
    bool needSwitch = (color != originalColor);
    
    if (needSwitch) {
        // This is a conceptual switch - in practice you might need to adjust
        // the current turn to evaluate from the other player's perspective
    }
    
    int score = 0;
    
    // Core strategic elements
    score += 300 * surround_score();
    score += 100 * mobility_score();
    score += 100 * tempo_score();
    score += 80 * control_score();
    score += 60 * pin_score();
    score += 50 * near_moves_score();
    score += 50 * bug_utility_score();
    
    // Individual piece contributions
    if (bothQueensPlaced()) {
        for (pieceT p = 1; p <= 28; p++) {
            if (G.isPlaced[p]) {
                int pieceScore = getScoreBug(p);
                if (col(p) == color) {
                    score += pieceScore;
                } else {
                    score -= pieceScore;
                }
            }
        }
    }
    
    return score;
    */
    return evaluateAdvanced(color);
}