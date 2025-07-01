#include "agent.h"
#include <vector>
#include <cstdlib> // For rand()
#include <ctime>   // For seeding random number generator

#include <thread>
#include <chrono>

using namespace std;

class RandomAgent : public Agent {
public:
    RandomAgent(int tl = 100000, int dl = 1000) : Agent(tl, dl) {
        srand(time(nullptr)); // Seed random number generator
    }

    action calculate_best_move(Board& board) override {
        vector<action> moves = board.possibleMoves(); // Assuming this function exists
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second. Used for Mzinga Viewer, TODO remove

        int random_index = rand() % moves.size(); // Select a random move
        return moves[random_index];
    }
};
