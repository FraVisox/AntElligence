#include "board.h"

// =============================================================================
// ADVANCED EVALUATION IMPLEMENTATION
// =============================================================================


/**
 * Calculate detailed metrics for a single piece
 */
BugMetrics Board::calculatePieceMetrics(pieceT piece) {
    BugMetrics metrics;
    
    if (!G.isPlaced[piece]) {
        return metrics; // All fields remain false/0
    }
    
    metrics.InPlay = true;
    metrics.IsPinned = isPinned(piece);
    metrics.IsCovered = isCovered(piece);
    metrics.IsOnTop = G.isTop(piece);
    
    PlayerColor pieceColor = col(piece);
    PlayerColor myColor = currentColor();
    positionT piecePos = G.getPosition(piece);
    
    // Count neighbors
    for (int dir = 0; dir < 6; dir++) {
        positionT adjPos = applayMove(piecePos, dir);
        if (!G.isFree(adjPos)) {
            pieceT adjPiece = G.topPiece(adjPos);
            if (col(adjPiece) == pieceColor) {
                metrics.FriendlyNeighbors+=1;
            } else {
                metrics.EnemyNeighbors+=1;
            }
            
            // Check for ant or beetle adjacency (for mosquito bonus)
            BugType adjType = kind(adjPiece);
            if (adjType == SOLDIER_ANT || adjType == BEETLE) {
                metrics.AdjacentToAntOrBeetle = true;
            }
        }
    }
    
    // Check proximity to own queen
    if (bothQueensPlaced()) {
        pieceT ownQueen = (pieceColor == PlayerColor::WHITE) ? 8 : 22;
        positionT ownQueenPos = G.getPosition(ownQueen);
        
        if (getHexDistance(piecePos, ownQueenPos) <= 2) {
            metrics.NearOwnQueen = true;
        }
        
        // Check if piece can reach a hole around enemy queen (for ladybug)
        if (kind(piece) == LADYBUG) {
            pieceT enemyQueen = (pieceColor == PlayerColor::WHITE) ? 22 : 8;
            positionT enemyQueenPos = G.getPosition(enemyQueen);
            
            // Check if there are free spaces around enemy queen
            for (int dir = 0; dir < 6; dir++) {
                positionT adjPos = applayMove(enemyQueenPos, dir);
                if (G.isFree(adjPos) && getHexDistance(piecePos, adjPos) <= 3) {
                    metrics.CanReachQueenHole = true;
                    break;
                }
            }
        }
    }
    
    // Calculate move counts (simplified - you might want to make this more sophisticated)
    if (!metrics.IsPinned && !metrics.IsCovered) {
        
        // Classify moves as noisy (near enemy queen) or quiet (positional)
        if (bothQueensPlaced()) {
            ComputePossibleMoves();
        
            pieceT enemyQueen = (pieceColor == PlayerColor::WHITE) ? 22 : 8;
            positionT enemyQueenPos = G.getPosition(enemyQueen);
            
            for (int i = idxStartActions[piece]; i <  idxStartActions[piece+1]; i++) {
                actionT action = resAction[i];
                if (action == 0) continue;
                
                positionT destPos = action / 32;
                if (getHexDistance(destPos, enemyQueenPos) <= 2) {
                    metrics.NoisyMoves++;
                } else {
                    metrics.QuietMoves++;
                }
            }
        }
        
        // Restore original move list
    }
    
    return metrics;
}

/**
 * Calculate comprehensive board metrics
 */

BoardMetrics Board::calculateBoardMetrics() {
    BoardMetrics boardMetrics;
    
    PlayerColor myColor = currentColor();
    
    // Calculate metrics for all pieces
    for (pieceT p = 1; p <= 28; p++) {
        boardMetrics.pieceMetrics[p] = calculatePieceMetrics(p);
        continue;
        if (G.isPlaced[p]) {
            boardMetrics.PiecesInPlay++;
        } else if (inHandPiece[p]) {
            boardMetrics.PiecesInHand++;
        }
        
        // Count pinned pieces
        if (isPinned(p)) {
            if (col(p) == myColor) {
                boardMetrics.MyPinCount++;
            } else {
                boardMetrics.TheirPinCount++;
            }
        }
    }
    return boardMetrics;
    // Queen-specific metrics
    if (bothQueensPlaced()) {
        pieceT myQueen = getMyQueen();
        pieceT theirQueen = getOpposingQueen();
        
        positionT myQueenPos = G.getPosition(myQueen);
        positionT theirQueenPos = G.getPosition(theirQueen);
        
        // Count surrounded spaces and escape routes
        for (int dir = 0; dir < 6; dir++) {
            positionT myQueenAdj = applayMove(myQueenPos, dir);
            positionT theirQueenAdj = applayMove(theirQueenPos, dir);
            
            if (!G.isFree(myQueenAdj)) {
                boardMetrics.MyQueenSurround++;
            } else {
                boardMetrics.MyQueenEscape++;
            }
            
            if (!G.isFree(theirQueenAdj)) {
                boardMetrics.TheirQueenSurround++;
            } else {
                boardMetrics.TheirQueenEscape++;
            }
        }
        
        // Check if queens are pinned
        boardMetrics.MyQueenPinned = isPinned(myQueen);
        boardMetrics.EnemyQueenPinned = isPinned(theirQueen);
        
        // Check win conditions
        boardMetrics.CanWinThisTurn = (boardMetrics.TheirQueenSurround == 6);
        boardMetrics.OpponentCanWinNextTurn = (boardMetrics.MyQueenSurround >= 5);
        
        // Check if we can place to win (simplified)
        boardMetrics.CanPlaceToWin = (boardMetrics.TheirQueenSurround == 5) && 
                                    (boardMetrics.PiecesInHand > 0);
        boardMetrics.OpponentCanPlaceToWin = (boardMetrics.MyQueenSurround == 5);
    }
    
    return boardMetrics;
}

/**
 * Calculate early game positional score
 */
double Board::calculateEarlyGamePositionalScore() {
    double score = 0.0;
    
    // Early game focuses on piece development and queen safety
    if (!bothQueensPlaced()) {
        return score;
    }
    
    PlayerColor myColor = currentColor();
    positionT myQueenPos = G.getPosition(getMyQueen());
    
    // Reward pieces near the center of action
    for (pieceT p = 1; p <= 28; p++) {
        if (G.isPlaced[p] && col(p) == myColor) {
            positionT piecePos = G.getPosition(p);
            
            // Reward pieces that maintain connection
            int connections = 0;
            for (int dir = 0; dir < 6; dir++) {
                positionT adjPos = applayMove(piecePos, dir);
                if (!G.isFree(adjPos)) {
                    connections++;
                }
            }
            score += connections * 5.0;
            
            // Penalize pieces too far from queen
            int distanceToQueen = getHexDistance(piecePos, myQueenPos);
            if (distanceToQueen > 3) {
                score -= distanceToQueen * 3.0;
            }
        }
    }
    
    return score;
}

/**
 * Calculate late game threat score
 */
double Board::calculateLateGameThreatScore() {
    double score = 0.0;
    
    if (!bothQueensPlaced()) {
        return score;
    }
    
    // Late game focuses on queen threats and mobility
    PlayerColor myColor = currentColor();
    positionT enemyQueenPos = G.getPosition(getOpposingQueen());
    
    // Reward pieces that can threaten enemy queen
    for (pieceT p = 1; p <= 28; p++) {
        if (G.isPlaced[p] && col(p) == myColor && !isPinned(p)) {
            positionT piecePos = G.getPosition(p);
            int distanceToEnemyQueen = getHexDistance(piecePos, enemyQueenPos);
            
            if (distanceToEnemyQueen <= 2) {
                // Pieces near enemy queen are valuable
                BugType type = kind(p);
                switch (type) {
                    case SOLDIER_ANT: score += 30.0; break;
                    case BEETLE: score += 25.0; break;
                    case GRASSHOPPER: score += 20.0; break;
                    case LADYBUG: score += 25.0; break;
                    default: score += 15.0; break;
                }
            }
        }
    }
    
    return score;
}

/**
 * Advanced board evaluation function
 */
double Board::evaluateAdvanced() {
    BoardMetrics boardMetrics = calculateBoardMetrics();
    
    PlayerColor color=currentColor();
    double score = 0.0;
    
    // Evaluate each piece
    for (pieceT p = 1; p <= 28; p++) {
        
        const BugMetrics& m = boardMetrics.pieceMetrics[p];
        
        double sign = (col(p) == color) ? +1.0 : -1.0;
        BugType type = kind(p);
        
        score += sign * getWeight(type, InPlayWeight   ) * (m.InPlay ? 1 : 0);
        score += sign * getWeight(type, IsPinnedWeight ) * (m.IsPinned ? 1 : 0);
        score += sign * getWeight(type, IsCoveredWeight) * (m.IsCovered ? 1 : 0);
        
        score += sign * getWeight(type, NoisyMoveWeight) * std::log1p(m.NoisyMoves);
        score += sign * getWeight(type, QuietMoveWeight) * std::sqrt(m.QuietMoves);
        
        score += sign * getWeight(type, FriendlyNeighborWeight)* m.FriendlyNeighbors;
        score += sign * getWeight(type, EnemyNeighborWeight) * m.EnemyNeighbors;
        
    }
    return score;
    // Global tactical features
    int queenSurroundDiff, queenEscapeDiff, pinDiff;
    
    if (color == currentColor()) {
        queenSurroundDiff = boardMetrics.TheirQueenSurround - boardMetrics.MyQueenSurround;
        queenEscapeDiff = boardMetrics.TheirQueenEscape - boardMetrics.MyQueenEscape;
        pinDiff = boardMetrics.TheirPinCount - boardMetrics.MyPinCount;
    } else {
        queenSurroundDiff = boardMetrics.MyQueenSurround - boardMetrics.TheirQueenSurround;
        queenEscapeDiff = boardMetrics.MyQueenEscape - boardMetrics.TheirQueenEscape;
        pinDiff = boardMetrics.MyPinCount - boardMetrics.TheirPinCount;
    }
    
    score += 200.0 * queenSurroundDiff;
    score += 15.0 * queenEscapeDiff;
    score += 10.0 * pinDiff;
    
    // Win/loss conditions
    
    // Queen pinning bonuses
    bool enemyQueenPinned = boardMetrics.EnemyQueenPinned;
    bool myQueenPinned =   boardMetrics.MyQueenPinned ;
        
    if (enemyQueenPinned) score += 150.0;
    if (myQueenPinned) score -= 150.0;
    
    // Placement win opportunities
    bool canPlaceToWin = (color == currentColor()) ? 
        boardMetrics.CanPlaceToWin : boardMetrics.OpponentCanPlaceToWin;
    bool opponentCanPlaceToWin = (color == currentColor()) ? 
        boardMetrics.OpponentCanPlaceToWin : boardMetrics.CanPlaceToWin;
        
    if (canPlaceToWin) score += 500.0;
    if (opponentCanPlaceToWin) score -= 500.0;
    
    // Early vs late game blend
    int inHand = boardMetrics.PiecesInHand;
    int inPlay = boardMetrics.PiecesInPlay;
    double earlyRatio = inHand / double(inHand + inPlay + 1);
    double lateRatio = 1.0 - earlyRatio;
    
    double earlyScore = calculateEarlyGamePositionalScore();
    double lateScore = calculateLateGameThreatScore();
    
    score += (earlyRatio * earlyScore) + (lateRatio * lateScore);
    
    return score;
}