#ifndef MCTS_H
#define MCTS_H

#include <memory>
#include <vector>
#include <torch/script.h>
#include <torch/torch.h>
#include "node.h"
#include "args.h"

// Forward declaration of the board state type
class EBoard;

// Monte Carlo Tree Search orchestration class
class MCTS {
public:
    // Construct with a TorchScript model
    explicit MCTS(const std::string& model_path);
    // Perform search from root_state; returns a vector of action probabilities of length MAX_ACTIONS
    std::vector<double> search(EBoard* root_state, int turn = 1);
   
private:
    // Underlying model for evaluations
    torch::jit::script::Module model_;
};

#endif // MCTS_H
