#include "board.h"


// =============================================================================
// ADVANCED EVALUATION IMPLEMENTATION
// =============================================================================

/**
 * Initialize default weights for different piece types and metrics
 */
MetricWeights Board::initializeDefaultWeights() {
    MetricWeights weights;
    
    // Weight format: BugType_MetricName
    BugType types[] = {QUEEN, SPIDER, BEETLE, GRASSHOPPER, SOLDIER_ANT, MOSQUITO, LADYBUG, PILLBUG};
    
    for (BugType type : types) {
        // Base weights - adjust these based on piece importance
        double baseWeight = 1.0;
        switch (type) {
            case QUEEN:      baseWeight = 5.0; break;
            case SOLDIER_ANT: baseWeight = 3.0; break;
            case BEETLE:     baseWeight = 2.5; break;
            case GRASSHOPPER: baseWeight = 2.0; break;
            case MOSQUITO:   baseWeight = 2.5; break;
            case LADYBUG:    baseWeight = 2.0; break;
            case SPIDER:     baseWeight = 1.5; break;
            case PILLBUG:    baseWeight = 2.0; break;
        }
        
        weights.Set(type, "InPlayWeight", baseWeight * 10.0);
        weights.Set(type, "IsPinnedWeight", -baseWeight * 20.0);
        weights.Set(type, "IsCoveredWeight", -baseWeight * 15.0);
        weights.Set(type, "IsOnTopWeight", baseWeight * 5.0);
        
        weights.Set(type, "NoisyMoveWeight", baseWeight * 3.0);
        weights.Set(type, "QuietMoveWeight", baseWeight * 1.0);
        
        weights.Set(type, "FriendlyNeighborWeight", baseWeight * 2.0);
        weights.Set(type, "EnemyNeighborWeight", -baseWeight * 1.5);
        
        // Special bonuses
        if (type == MOSQUITO) {
            weights.Set(type, "MosquitoPowerBonus", 25.0);
        }
        if (type == PILLBUG) {
            weights.Set(type, "DefensivePillbugBonus", 20.0);
        }
        if (type == LADYBUG) {
            weights.Set(type, "QueenDiveBonus", 30.0);
        }
    }
    
    return weights;
}

/**
 * Convert piece ID to string identifier
 */
std::string Board::pieceToString(pieceT piece) {
    PlayerColor color = col(piece);
    BugType type = kind(piece);
    int number = numInc(piece);
    
    std::string colorStr = (color == PlayerColor::WHITE) ? "W" : "B";
    std::string typeStr;
    
    switch (type) {
        case QUEEN: typeStr = "Q"; break;
        case SPIDER: typeStr = "S"; break;
        case BEETLE: typeStr = "B"; break;
        case GRASSHOPPER: typeStr = "G"; break;
        case SOLDIER_ANT: typeStr = "A"; break;
        case MOSQUITO: typeStr = "M"; break;
        case LADYBUG: typeStr = "L"; break;
        case PILLBUG: typeStr = "P"; break;
    }
    
    return colorStr + typeStr + std::to_string(number);
}

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
                metrics.FriendlyNeighbors++;
            } else {
                metrics.EnemyNeighbors++;
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
        // Temporarily compute moves for this piece to count noisy vs quiet
        // This is expensive but gives accurate move classification
        int originalNumAction = numAction;
        actionT originalActions[MAX_ACTIONS_SIZE];
        std::copy(resAction, resAction + numAction, originalActions);
        
        possibleMovesBug(piece);
        
        // Classify moves as noisy (near enemy queen) or quiet (positional)
        if (bothQueensPlaced()) {
            pieceT enemyQueen = (pieceColor == PlayerColor::WHITE) ? 22 : 8;
            positionT enemyQueenPos = G.getPosition(enemyQueen);
            
            for (int i = originalNumAction; i < numAction; i++) {
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
        numAction = originalNumAction;
        std::copy(originalActions, originalActions + numAction, resAction);
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
        std::string pieceName = pieceToString(p);
        boardMetrics.pieceMetrics[pieceName] = calculatePieceMetrics(p);
        
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
double Board::evaluateAdvanced(PlayerColor playerColor) {
    BoardMetrics boardMetrics = calculateBoardMetrics();
    MetricWeights weights = initializeDefaultWeights();
    
    double score = 0.0;
    
    // Evaluate each piece
    for (pieceT p = 1; p <= 28; p++) {
        std::string pieceName = pieceToString(p);
        const BugMetrics& m = boardMetrics.pieceMetrics[pieceName];
        
        int sign = (col(p) == playerColor) ? +1 : -1;
        BugType type = kind(p);
        
        score += sign * weights.Get(type, "InPlayWeight") * (m.InPlay ? 1 : 0);
        score += sign * weights.Get(type, "IsPinnedWeight") * (m.IsPinned ? 1 : 0);
        score += sign * weights.Get(type, "IsCoveredWeight") * (m.IsCovered ? 1 : 0);
        score += sign * weights.Get(type, "IsOnTopWeight") * (m.IsOnTop ? 1 : 0);
        
        score += sign * weights.Get(type, "NoisyMoveWeight") * std::log1p(m.NoisyMoves);
        score += sign * weights.Get(type, "QuietMoveWeight") * std::sqrt(m.QuietMoves);
        
        score += sign * weights.Get(type, "FriendlyNeighborWeight") * m.FriendlyNeighbors;
        score += sign * weights.Get(type, "EnemyNeighborWeight") * m.EnemyNeighbors;
        
        // Special bonuses
        if (type == MOSQUITO && m.AdjacentToAntOrBeetle) {
            score += sign * weights.Get(type, "MosquitoPowerBonus");
        }
        
        if (type == PILLBUG && m.NearOwnQueen) {
            score += sign * weights.Get(type, "DefensivePillbugBonus");
        }
        
        if (type == LADYBUG && m.CanReachQueenHole) {
            score += sign * weights.Get(type, "QueenDiveBonus");
        }
    }
    
    // Global tactical features
    int queenSurroundDiff, queenEscapeDiff, pinDiff;
    
    if (playerColor == currentColor()) {
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
    bool canWin = (playerColor == currentColor()) ? 
        boardMetrics.CanWinThisTurn : boardMetrics.OpponentCanWinNextTurn;
    bool opponentCanWin = (playerColor == currentColor()) ? 
        boardMetrics.OpponentCanWinNextTurn : boardMetrics.CanWinThisTurn;
        
    if (canWin) return 1e6;
    if (opponentCanWin) return -1e6;
    
    // Queen pinning bonuses
    bool enemyQueenPinned = (playerColor == currentColor()) ? 
        boardMetrics.EnemyQueenPinned : boardMetrics.MyQueenPinned;
    bool myQueenPinned = (playerColor == currentColor()) ? 
        boardMetrics.MyQueenPinned : boardMetrics.EnemyQueenPinned;
        
    if (enemyQueenPinned) score += 150.0;
    if (myQueenPinned) score -= 150.0;
    
    // Placement win opportunities
    bool canPlaceToWin = (playerColor == currentColor()) ? 
        boardMetrics.CanPlaceToWin : boardMetrics.OpponentCanPlaceToWin;
    bool opponentCanPlaceToWin = (playerColor == currentColor()) ? 
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