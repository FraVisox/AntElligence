#include "mcts/mcts.h"
#include "engine/embadded_board.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <numeric>
#include <cmath>

static std::mt19937_64 RNG{ std::random_device{}() };

struct History {
    torch::Tensor xm;
     torch::Tensor xd;
    std::vector<double> action_probs;
    int outcome;
};

// each returned entry is: (board_tensor, metadata_tensor, action_probs, outcome)
using ReturnType = std::tuple<torch::Tensor, torch::Tensor,
                              std::vector<double>, int>;
int main(){
    string model_name;
    cin>>model_name;
    
    int turn_sp = 0;
    std::vector<History> memory;
    int player = 1;

    MCTS mcts(model_name);

    // Start from initial state
    EBoard* state=new EBoard((GameType)7);

    while (turn_sp<7) {
        // 1) make a neutral copy for MCTS
        EBoard* neutral_state = new EBoard(state);


        auto action_probs = mcts.search(neutral_state);

        turn_sp++;

        neutral_state ->updateVectRapp();
        auto t=torch::from_blob(
            neutral_state->vectRapp,                           // data pointer
            {1033},                         // tensor shape
            torch::TensorOptions().dtype(torch::kFloat));
        
        memory.push_back({ t.narrow(0, /*start=*/0, /*length=*/1024) .view({1, 32, 32}),
                            t.narrow(0, /*start=*/1024, /*length=*/t.size(0) - 1024),
                            action_probs,
                            player });

        double inv_temp = 1.0 / ARG_temperature;
        std::vector<double> temp_probs(action_probs.size());
        for (size_t i = 0; i < action_probs.size(); ++i) {
            temp_probs[i] = std::pow(action_probs[i], inv_temp);
        }
        double sum_tp = std::accumulate(temp_probs.begin(), temp_probs.end(), 0.0);
        if (sum_tp > 0) {
            for (auto &p : temp_probs) p /= sum_tp;
        } else {
            double u = 1.0 / temp_probs.size();
            std::fill(temp_probs.begin(), temp_probs.end(), u);
        }

        std::discrete_distribution<int> dist(temp_probs.begin(), temp_probs.end());
        int idx = dist(RNG);

        // map to actual game move
        state->board_exp.ComputePossibleMoves();
        auto moves=state ->board_exp.G.associatedAction;
        int action = moves[idx];

        state->applyAction(action);
        
        // 5) check for terminal
        int value = state->getState();

        if (value>1) {
            // build return memory
            std::vector<ReturnType> result;
            for (auto &h : memory) {
                h.outcome = (h.outcome == player)
                            ? value
                            : -value;

                // get tensors for training
                
            }
            delete state;
            return 0;
        }

        // 6) next turn
        player = -(player);
    }
}