#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

// --- Example Types ---

enum BugType {
    ANT, HOPPER, BEETLE, SPIDER, QUEEN, MOSQUITO, LADYBUG, PILLBUG
};

enum MoveType {
    PLACE, SLIDE, HOP, CLIMB, PILLBUG_LIFT
};

struct Coord {
    int x, y;
    int distance_to(const Coord& other) const {
        return std::max({ std::abs(x - other.x), std::abs(y - other.y), std::abs(x + y - other.x - other.y) });
    }
};

struct Move {
    MoveType move_type;
    BugType bug_type;
    Coord from;
    Coord to;
    bool is_pinning_move = false;
    bool is_winning_move = false;
    bool climbs_on_enemy_queen = false;
    bool threatens_to_win_next = false;

    std::string to_string() const {
        return "[Move " + std::to_string(bug_type) + " to (" + std::to_string(to.x) + "," + std::to_string(to.y) + ")]";
    }
};

// --- Move Evaluation ---

int evaluate_move_priority(const Move& move) {
    int score = 0;

    if (move.is_winning_move)
        return 100000;

    if (move.climbs_on_enemy_queen)
        score += 900;

    if (move.threatens_to_win_next)
        score += 700;

    if (move.is_pinning_move)
        score += 600;

    // Bug value
    switch (move.bug_type) {
        case ANT: score += 500; break;
        case BEETLE: score += 400; break;
        case HOPPER: score += 300; break;
        case MOSQUITO: score += 300; break;
        case LADYBUG: score += 250; break;
        case SPIDER: score += 150; break;
        case PILLBUG: score += 100; break;
        case QUEEN: score -= 100; break; // Avoid moving Queen if possible
    }

    // Move type bonus
    switch (move.move_type) {
        case PLACE: score += 50; break;
        case SLIDE: score += 30; break;
        case HOP: score += 40; break;
        case CLIMB: score += 70; break;
        case PILLBUG_LIFT: score += 60; break;
    }

    return score;
}

// --- Ordering Function ---

void order_moves(std::vector<Move>& moves) {
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return evaluate_move_priority(a) > evaluate_move_priority(b);
    });
}

// --- Test Stub ---

int main() {
    std::vector<Move> moves = {
        { SLIDE, ANT, {1, 1}, {1, 2}, false, false, false, true },
        { CLIMB, BEETLE, {2, 2}, {2, 1}, false, false, true, false },
        { PLACE, HOPPER, {-1, 0}, {0, 1}, false, false, false, false },
        { PLACE, MOSQUITO, {-1, -1}, {0, 0}, false, true, false, false },
        { PILLBUG_LIFT, PILLBUG, {0, 0}, {1, 1}, true, false, false, false }
    };

    std::cout << "Before ordering:\n";
    for (const auto& move : moves) {
        std::cout << move.to_string() << "\n";
    }

    order_moves(moves);

    std::cout << "\nAfter ordering:\n";
    for (const auto& move : moves) {
        std::cout << move.to_string() << "\n";
    }

    return 0;
}
