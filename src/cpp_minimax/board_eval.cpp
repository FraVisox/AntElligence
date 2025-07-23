#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>

/*
EVALUATE A BOARD:
Score = α × Mobility + β × Tempo Advantage + γ × Strength + δ × Queen Danger + ε × Pin Pressure
Here’s a breakdown:

1. Mobility (α):

Count mobile bugs on each side. More mobile bugs = better flexibility.
Weight Ants highest (most mobile), then Hopper, Ladybug, Beetle, Mosquito, Spider.

2. Tempo Advantage (β):

Estimated turns to win or defend.
Count attacking bugs within 2 moves of enemy queen and defending bugs next to own queen.

3. Strength (γ):

Number of bugs in play and their usefulness.
Evaluate number of accessible spaces around opponent's queen vs. number of your bugs that can reach them.

4. Queen Danger (δ):

Bonus/malus based on number of hexes around your queen filled (bad) vs. around opponent’s queen (good).
Penalize if your queen is near corner/edge with fewer escape paths.

5. Pin Pressure (ε):

Positive if your pins reduce enemy mobility or freeze attackers.
Negative if your own key bugs are pinned.


*/

enum BugType {
    ANT, HOPPER, BEETLE, SPIDER, QUEEN, MOSQUITO, LADYBUG, PILLBUG
};

struct Coord {
    int x, y;
    int distance_to(const Coord& other) const {
        return std::max({ std::abs(x - other.x), std::abs(y - other.y), std::abs(x + y - other.x - other.y) });
    }
};

struct Bug {
    BugType type;
    Coord position;
    bool pinned;
    bool movable;
    bool on_top;
    std::vector<BugType> adjacent_bugs;

    bool is_pinned() const { return pinned; }
    bool can_move() const { return movable && !pinned; }
    bool adjacent_to(BugType t) const {
        for (BugType b : adjacent_bugs)
            if (b == t) return true;
        return false;
    }

    bool near(const Coord& other) const {
        return position.distance_to(other) <= 1;
    }

    int distance_to(const Coord& other) const {
        return position.distance_to(other);
    }
};

struct Player {
    std::string name;
    bool is_white;
    Coord queen_pos;
};

struct Board {
    std::vector<Bug> white_bugs;
    std::vector<Bug> black_bugs;
    Coord white_queen_pos;
    Coord black_queen_pos;

    const std::vector<Bug>& get_bugs(const Player& p) const {
        return p.is_white ? white_bugs : black_bugs;
    }

    const std::vector<Bug>& get_enemy_bugs(const Player& p) const {
        return p.is_white ? black_bugs : white_bugs;
    }

    Coord my_queen(const Player& p) const {
        return p.is_white ? white_queen_pos : black_queen_pos;
    }

    Coord enemy_queen(const Player& p) const {
        return p.is_white ? black_queen_pos : white_queen_pos;
    }

    int count_surrounding(const Coord& pos) const {
        // Placeholder: actual implementation should check all 6 surrounding hexes.
        return 0;
    }

    bool is_occupied_by(const Player& p, const Coord& pos) const {
        for (auto& bug : get_bugs(p)) {
            if (bug.position.x == pos.x && bug.position.y == pos.y)
                return true;
        }
        return false;
    }

    std::vector<Coord> get_adjacent_spaces(const Coord& pos) const {
        return {
            {pos.x + 1, pos.y}, {pos.x - 1, pos.y},
            {pos.x, pos.y + 1}, {pos.x, pos.y - 1},
            {pos.x + 1, pos.y - 1}, {pos.x - 1, pos.y + 1}
        };
    }
};

// --- Evaluation Function Components ---

int surround_score(const Board& board, const Player& me) {
    int me_surround = board.count_surrounding(board.my_queen(me));
    int opp_surround = board.count_surrounding(board.enemy_queen(me));
    return (opp_surround * 15) - (me_surround * 20);
}

int mobility_score(const Board& board, const Player& me) {
    int score = 0;
    for (auto& bug : board.get_bugs(me)) {
        if (!bug.is_pinned() && bug.can_move()) {
            switch (bug.type) {
                case ANT: score += 10; break;
                case HOPPER: score += 8; break;
                case MOSQUITO: score += 8; break;
                case BEETLE: score += 6; break;
                case LADYBUG: score += 5; break;
                case SPIDER: score += 3; break;
                case PILLBUG: score += 1; break;
                default: break;
            }
        }
    }
    return score;
}

int tempo_score(const Board& board, const Player& me) {
    int score = 0;
    Coord enemy_queen = board.enemy_queen(me);
    for (auto& bug : board.get_bugs(me)) {
        if (bug.can_move() && bug.distance_to(enemy_queen) <= 2)
            score += 6;
    }
    return score;
}

int control_score(const Board& board, const Player& me) {
    int score = 0;
    for (auto& hex : board.get_adjacent_spaces(board.enemy_queen(me))) {
        if (board.is_occupied_by(me, hex))
            score += 5;
    }
    return score;
}

int pin_score(const Board& board, const Player& me) {
    int score = 0;
    for (auto& bug : board.get_enemy_bugs(me)) {
        if (bug.is_pinned()) {
            if (bug.type == ANT || bug.type == BEETLE || bug.type == HOPPER)
                score += 10;
        }
    }
    for (auto& bug : board.get_bugs(me)) {
        if (bug.is_pinned() && (bug.type == ANT || bug.type == HOPPER))
            score -= 10;
    }
    return score;
}

int bug_utility_score(const Board& board, const Player& me) {
    int score = 0;
    Coord my_q = board.my_queen(me);
    Coord opp_q = board.enemy_queen(me);
    for (auto& bug : board.get_bugs(me)) {
        switch (bug.type) {
            case MOSQUITO:
                if (bug.adjacent_to(BEETLE) || bug.adjacent_to(ANT)) score += 5;
                break;
            case LADYBUG:
                if (bug.near(my_q)) score += 5;
                break;
            case PILLBUG:
                if (bug.near(my_q)) score += 6;
                if (bug.near(opp_q)) score += 8;
                break;
            default: break;
        }
    }
    return score;
}

// --- Final Evaluation Function ---

int evaluate_board(const Board& board, const Player& me) {
    int score = 0;

    score += 300 * surround_score(board, me);
    score += 100 * mobility_score(board, me);
    score += 100 * tempo_score(board, me);
    score += 80  * control_score(board, me);
    score += 60  * pin_score(board, me);
    score += 50  * bug_utility_score(board, me);

    return score;
}

// --- Test Driver (Example Stub) ---

int main() {
    Board board;
    Player white = { "White", true, {0, 0} };
    Player black = { "Black", false, {1, 0} };

    int eval = evaluate_board(board, white);
    std::cout << "White board evaluation: " << eval << std::endl;

    return 0;
}
