#ifndef NODE_H
#define NODE_H

#include <vector>
#include <random>
#include <numeric>
#include <torch/torch.h>

#include "args.h"
#include "../engine/embadded_board.h"

// Forward declaration
//class EBoard;
//using actionT = int;  // adjust if actionT is a typedef elsewhere

class Node {
public:
    // Constructors & Destructor
    Node(EBoard* state = nullptr,
         Node* parent = nullptr,
         int action_taken = -1,
         double prior = 0.0,
         int visit_count = 0,
         actionT     action = 0);
    ~Node();

    // Tree operations
    bool is_fully_expanded() const;
    Node* select();
    std::vector<Node*> selectMultiple(int k);
    void expand(const std::vector<double>& policy);
    void backpropagate(double value);
    void delete_nodes();

    // Evaluation & status
    std::pair<torch::Tensor, torch::Tensor> getVectSplit();
    int getStatus();

    // Accessors
    int  action_taken() const;
    int  visit_count()  const;

    // UCB computation
    double get_ucb() const;

    // Public data for final policy tally
    int expandable_sons;
    int inQueue_sons;


    // Node attributes
    EBoard*             state_;
    Node*               parent_;
    int                 action_taken_;
    actionT             action_;
    double              prior_;
    int                 visit_count_;
    double              value_sum_;
    bool                selected_;
    std::vector<Node*>  children_;
};

#endif // NODE_H
