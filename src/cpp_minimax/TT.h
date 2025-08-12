#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <vector>
#include <optional>



#include <atomic>
#include <cmath>
#include <cassert>

// ----- Entry types -----
enum class EntryType : uint8_t {
    Exact,      // exact value
    LowerBound, // value is a lower bound (alpha)
    UpperBound  // value is an upper bound (beta)
};

struct TTEntry {
    size_t key;             // full key for collision check
    uint64_t zobrist;      // optional extra fingerprint if using Zobrist (can be 0)
    double value;          // score
    int8_t depth;          // remaining depth at which this was stored
    EntryType type;        // exact / lower / upper
    int16_t bestMove;      // you can encode your move; adapt type if your move is more complex
    uint16_t age;          // for aging / replacement
};


class TranspositionTable {
public:
    explicit TranspositionTable(size_t size_power_of_two);

    // Probe: try to retrieve entry. Returns pointer if usable (matching key and depth), else nullptr.
    std::optional<TTEntry> probe(const size_t& key, double alpha, double beta, int depth) const ;
    // Always store (with replacement policy)
    void store(const size_t& key, double value, int depth, EntryType type, int16_t bestMove);
    // Increment age (call e.g. each root search iteration)
    void new_search_iteration();

    size_t capacity;
    size_t mask;
    std::vector<TTEntry> table;
    uint16_t ages; // global age tick

    uint16_t current_age() const ;

    size_t index_for_key(const size_t& h) const;
};
