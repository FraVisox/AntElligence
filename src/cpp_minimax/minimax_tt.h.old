#ifndef MINIMAXTT_H
#define MINIMAXTT_H

#include "engine/board.h"
#include "engine/enums.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>

#include <unordered_map>

// Transposition table entry structure
struct TTEntry {
    uint64_t hash;           // Board hash for verification
    int value;               // Stored evaluation
    int depth;               // Depth at which this was evaluated
    int flag;                // EXACT, LOWERBOUND, or UPPERBOUND
    actionT best_move;       // Best move found at this position
    
    TTEntry() : hash(0), value(0), depth(-1), flag(0), best_move(pass()) {}
    TTEntry(uint64_t h, int v, int d, int f, actionT move) 
        : hash(h), value(v), depth(d), flag(f), best_move(move) {}
};

// Transposition table flags
const int TT_EXACT = 0;      // Exact value
const int TT_LOWERBOUND = 1; // Alpha cutoff occurred (value >= beta)
const int TT_UPPERBOUND = 2; // Beta cutoff occurred (value <= alpha)

class TranspositionTable {
private:
    std::unordered_map<uint64_t, TTEntry> table;
    size_t max_entries;
    
public:
    TranspositionTable(size_t max_size = 1000000) : max_entries(max_size) {}
    
    void store(uint64_t hash, int value, int depth, int flag, actionT best_move) {
        // If table is getting too large, clear it (simple replacement strategy)
        if (table.size() >= max_entries) {
            table.clear();
        }
        
        auto it = table.find(hash);
        if (it == table.end() || it->second.depth <= depth) {
            // Store if not present or if we have a deeper search
            table[hash] = TTEntry(hash, value, depth, flag, best_move);
        }
    }
    
    bool lookup(uint64_t hash, int depth, int alpha, int beta, int& value, actionT& best_move) {
        auto it = table.find(hash);
        if (it == table.end() || it->second.hash != hash) {
            return false; // Not found or hash collision
        }
        
        const TTEntry& entry = it->second;
        best_move = entry.best_move;
        
        if (entry.depth >= depth) {
            if (entry.flag == TT_EXACT) {
                value = entry.value;
                return true;
            }
            if (entry.flag == TT_LOWERBOUND && entry.value >= beta) {
                value = entry.value;
                return true;
            }
            if (entry.flag == TT_UPPERBOUND && entry.value <= alpha) {
                value = entry.value;
                return true;
            }
        }
        
        return false; // Found but can't use the value
    }
    
    void clear() {
        table.clear();
    }
    
    size_t size() const {
        return table.size();
    }
};


class MinimaxAgentTT {
private:
    int calledBoard;
    PlayerColor color;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    const std::chrono::seconds TIME_LIMIT = std::chrono::seconds(5);

    actionT _cache = pass();
    int cached_turn = 1;
    bool DISABLE_CACHE = true;

    int timeLimit = 5;
    int depthLimit = 4;

    const int MIN_EVAL = -10000;
    const int MAX_EVAL = 10000;

    int utility(GameState state, Board board);
    actionT initiate_minimax_iterative(Board board);
    actionT initiate_minimax_fixed(Board board);
    int minmax(GameState state, Board board, int depth, int alpha, int beta);

    bool is_time_up() const {
        return std::chrono::high_resolution_clock::now() - start_time >= TIME_LIMIT;
    }

    TranspositionTable tt;

public:
    MinimaxAgentTT(int tl = 100000, int dl = 5) : color(PlayerColor::WHITE) {
        timeLimit = tl;
        depthLimit = dl;
    }

    actionT calculate_best_move(Board board);
};

#endif