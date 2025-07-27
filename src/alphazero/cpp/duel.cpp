// duel.cpp (Parallelized)
#include "mcts/mcts.h"
#include "engine/embadded_board.h"
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <thread>
#include <mutex>
#include <future>

std::mutex result_mutex;

int simulate_game(MCTS* mcts1, MCTS* mcts2, int iter) {
    int player = 1;
    int turn = 1;
    EBoard* state = new EBoard((GameType)7);

    while (true) {
        EBoard* neutral_state = new EBoard(state);
        std::vector<double> action_probs;

        if (player == 1)
            action_probs = mcts1->search(neutral_state);
        else
            action_probs = mcts2->search(neutral_state);

        state->board_exp.ComputePossibleMoves();
        auto moves = state->board_exp.G.associatedAction;

        int idx = std::distance(action_probs.begin(), std::max_element(action_probs.begin(), action_probs.end()));
        int action = moves[idx];
        state->applyAction(action);

        int value = state->getState();
        if (value > 1) {
            delete state;
            if (value == 4) return 0; // draw
            else if ((value == 2 && player == 1) || (value == 3 && player == -1)) return 1; // mcts1 wins
            else return 2; // mcts2 wins
        }

        player = -player;
        ++turn;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <model_1_path> <model_2_path>\n";
        return 1;
    }

    std::string model1_path = argv[1];
    std::string model2_path = argv[2];

    MCTS mcts1(model1_path);
    MCTS mcts2(model2_path);

    const int num_games = 10;
    int player_1_wins = 0;
    int player_2_wins = 0;
    int draws = 0;

    std::vector<std::future<int>> futures;
    for (int i = 0; i < num_games; ++i) {
        futures.push_back(std::async(std::launch::async, simulate_game, &mcts1, &mcts2, i));
    }

    for (auto& f : futures) {
        int result = f.get();
        std::lock_guard<std::mutex> lock(result_mutex);
        if (result == 0) ++draws;
        else if (result == 1) ++player_1_wins;
        else if (result == 2) ++player_2_wins;
    }

    std::cout << "Player 1 (" << model1_path << ") wins: " << player_1_wins << std::endl;
    std::cout << "Player 2 (" << model2_path << ") wins: " << player_2_wins << std::endl;
    std::cout << "Draws: " << draws << std::endl;

    std::string winner;
    if (player_1_wins > player_2_wins) winner = model1_path;
    else if (player_2_wins > player_1_wins) winner = model2_path;
    else winner = "draw";

    std::cout << "Winner: " << winner << std::endl;
    return 0;
}
