// Self-play.cpp (Modified to match Python selfPlay() output format and take input parameters)
#include "mcts/mcts.h"
#include "engine/embadded_board.h"
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <numeric>
#include <cmath>

static std::mt19937_64 RNG{std::random_device{}()};

using ReturnType = std::tuple<torch::Tensor, torch::Tensor, std::vector<double>, int>;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <model_name> <output_file>" << std::endl;
        return 1;
    }

    std::string model_name = argv[1];
    std::string output_file = argv[2];

    MCTS mcts(model_name);

    std::vector<ReturnType> memory;
    int player = 1;

    // Start from initial state
    EBoard* state = new EBoard((GameType)7);
    int turn_sp = 0;
    while(true) {
        turn_sp++;
        cout<<"Start turn "<< turn_sp<<endl;
        EBoard* neutral_state = new EBoard(state);
        auto action_probs = mcts.search(neutral_state);

        state->updateVectRapp();

        auto t = torch::from_blob(state->vectRapp, {1033}, torch::kFloat).clone();

        auto board_tensor = t.narrow(0, 0, 1024).view({1, 32, 32});
        auto metadata_tensor = t.narrow(0, 1024, 9);

        memory.emplace_back(board_tensor, metadata_tensor, action_probs, player);

        // Select action according to temperature
        double inv_temp = 1.0 / ARG_temperature;
        std::vector<double> temp_probs(action_probs.size());
        for (size_t i = 0; i < action_probs.size(); ++i)
            temp_probs[i] = std::pow(action_probs[i], inv_temp);

        double sum_tp = std::accumulate(temp_probs.begin(), temp_probs.end(), 0.0);
        if (sum_tp > 0)
            for (auto &p : temp_probs) p /= sum_tp;
        else
            std::fill(temp_probs.begin(), temp_probs.end(), 1.0 / temp_probs.size());

        std::discrete_distribution<int> dist(temp_probs.begin(), temp_probs.end());
        int idx = dist(RNG);

        state->board_exp.ComputePossibleMoves();
        int action = state->board_exp.G.associatedAction[idx];
        state->applyAction(action);

        int value = state->getState();

        if (value > 1) {
            std::cout << "Game ended with status " << value << std::endl;
            // Assign correct outcomes
            cout<<"Mem size:"<<memory.size()<<endl;
            for (auto &entry : memory) {
                int hist_player = std::get<3>(entry);
                int outcome = (hist_player == player) ? value : -value;
                std::get<3>(entry) = outcome;
            }

            // Convert memory to tensors for Python
            torch::serialize::OutputArchive archive;
                        
            std::vector<torch::Tensor> boards, metadata, action_probs_t, outcomes;

            for (auto &entry : memory) {
                cout<<"Add mentry in mem"<<endl;
                boards.push_back(std::get<0>(entry));
                metadata.push_back(std::get<1>(entry));
                action_probs_t.push_back(torch::tensor(std::get<2>(entry), torch::kFloat));
                outcomes.push_back(torch::tensor(std::get<3>(entry), torch::kInt32));
            }

            archive.write("boards", torch::stack(boards));
            archive.write("metadata", torch::stack(metadata));
            archive.write("action_probs", torch::stack(action_probs_t));
            archive.write("outcomes", torch::stack(outcomes));

            archive.save_to(output_file);
            cout<<"Archive saved in "<<output_file<<endl;
            delete state;
            return 0;
        }

        player = -player;
    }

    delete state;
    return 0;
}
