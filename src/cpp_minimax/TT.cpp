#include "TT.h"
#include <atomic>
#include <cmath>
#include <cassert>

// Simple fixed-size hash table with power-of-two capacity.
// Replacement: prefers deeper entries or newer (age) when collision.

     TranspositionTable::TranspositionTable(size_t size_power_of_two=0x1000000) {
        capacity = size_power_of_two;
        mask = capacity - 1;
        table.resize(capacity);
        ages = 0;
    }

    // Probe: try to retrieve entry. Returns pointer if usable (matching key and depth), else nullptr.
    std::optional<TTEntry> TranspositionTable::probe(const size_t& key, double alpha, double beta, int depth) const {
        size_t idx = index_for_key(key);
        const TTEntry& e = table[idx];

        if (e.depth < 0) return std::nullopt; // empty slot

        if (!(e.key == key)) return std::nullopt; // collision, different key

        // Depth check: we only use entries computed at >= remaining depth
        if (e.depth < depth) return std::nullopt;

        // Interpret based on entry type and bounds
        if (e.type == EntryType::Exact) {
            return e;
        } else if (e.type == EntryType::LowerBound && e.value >= beta) {
            return e; // beta cutoff
        } else if (e.type == EntryType::UpperBound && e.value <= alpha) {
            return e; // alpha cutoff
        }
        // Otherwise can't trust it for pruning, but might supply a best move later.
        return std::nullopt;
    }

    // Always store (with replacement policy)
    void TranspositionTable::store(const size_t& key, double value, int depth, EntryType type, int16_t bestMove) {
        size_t idx = index_for_key(key);
        TTEntry& e = table[idx];

        // Replacement logic: prefer deeper or newer (higher age), else keep existing.
        bool replace = false;
        if (e.depth < 0) {
            replace = true; // empty
        } else if (!(e.key == key)) {
            // different key: replace if new depth is >= existing depth, or if age is newer
            if (depth > e.depth) replace = true;
            else if (depth == e.depth && current_age() != e.age) replace = true;
        } else {
            // same key: overwrite if deeper or for exactness
            if (depth > e.depth) replace = true;
            else if (type == EntryType::Exact && e.type != EntryType::Exact) replace = true;
        }

        if (replace) {
            e.key = key;
            e.value = value;
            e.depth = static_cast<int8_t>(depth);
            e.type = type;
            e.bestMove = bestMove;
            e.age = current_age();
            // optional: e.zobrist left zero unless you integrate a secondary fingerprint
        }
    }

    // Increment age (call e.g. each root search iteration)
    void TranspositionTable::new_search_iteration() {
        ages = (ages + 1) & 0xFFFF; // wraparound safely in 16 bits
    }

    size_t capacity;
    size_t mask;
    std::vector<TTEntry> table;
    uint16_t ages; // global age tick

    uint16_t TranspositionTable::current_age() const {
        return ages;
    }

    size_t TranspositionTable::index_for_key(const size_t& h) const {
        return h & mask;
    }

