// MCTS.h
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <limits>
#include <torch/torch.h>
// -----------------------------------------------------------------------------
// Arguments struct (replace/add fields as needed)

// -----------------------------------------------------------------------------
// A single node in the search tree
#include "node.h"

Node::Node(EBoard* state,
       Node* parent,
       int action_taken ,
       double prior,
       int visit_count,
       actionT action):
      parent_(parent)
    , action_taken_(action_taken)
    , prior_(1)
    , action_(action)
    , visit_count_(visit_count)
    , value_sum_(0.0)
    , selected_(false)
  {
          state_=state;
  }

Node::~Node() { delete_nodes(); }

void Node::explicitateState(){
    if (!isExplicitState) {
        
      state_ = new EBoard(parent_->state_);
      state_ -> applyAction(action_);
        isExplicitState=true;
    }
}
    

bool Node::is_fully_expanded() const {
return !children_.empty();
}

  // pick the single best child by UCB
Node* Node::select() {
    Node* best = nullptr;
    double best_ucb = -1000;
    for (auto& cptr : children_) {
        Node* c = cptr;
        double u = c->get_ucb();
        if (u > best_ucb ) {
        best = c;
        best_ucb = u;
        }
    }
    return best;
}

std::vector<Node*> Node::selectMultiple(int k) {
    std::vector<Node*> result;
    selectMultipleHelper(k, result);
    return result;
}

void Node::selectMultipleHelper(int k, std::vector<Node*>& result) {
    if ((int)result.size() >= k)
        return;

    if (!selected_ && !is_fully_expanded()) {
        selected_ = true;  
        result.push_back(this);
        return;
    }

    if (children_.empty())  // No children to select
        return;

    // Compute UCBs and partially sort for top candidates (efficiency)
    std::vector<std::pair<double, Node*>> ucbs;
    ucbs.reserve(children_.size());
    for (auto& child : children_)
        ucbs.emplace_back(child->get_ucb(), child);

    // Partial sort to get highest UCB first
    std::partial_sort(ucbs.begin(), ucbs.begin() + std::min(k, (int)ucbs.size()), ucbs.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    for (int i = 0; i < (int)ucbs.size() && (int)result.size() < k; ++i) {
        ucbs[i].second->selectMultipleHelper(k, result);
    }
}

double Node::get_ucb() const {
    double q = 0.0;
    if (visit_count_ > 0) {
      double win_rate = value_sum_ / visit_count_;
      q = 1.0 - ((win_rate + 1.0) / 2.0);
    }
    double expl = ARG_C
      * (std::sqrt((double)parent_->visit_count_) / (visit_count_ + 1))
      * prior_;
    return q + expl;
}

  // lazily compute the feature tensors for this node
std::pair<torch::Tensor,torch::Tensor> Node::getVectSplit() {
    
    explicitateState();
    //cout << "Getting vect split" << endl;
    
    state_ ->updateVectRapp();
    auto t = torch::from_blob(state_->vectRapp, {1033}, torch::kShort).clone();
    auto board = t.narrow(0, 0, 1024).to(torch::kFloat).view({1, 32, 32});
    auto metadata = t.narrow(0, 1024, 9).to(torch::kFloat);
    
    return make_pair(board, metadata);
    //torch::Tensor t(state_->vectRapp);
}

// Expand all legal children according to `policy` vector
void Node::expand(const std::vector<double>& policy) {
    if (!children_.empty()) return;

    // ensure state_ is initialized
    explicitateState();
    state_->board_exp.ComputePossibleMoves();
    auto mask  = state_->board_exp.G.isValidMoveBitmask;    // std::vector<int> or Eigen/Array
    auto moves = state_->board_exp.G.associatedAction;

    // elementwise mask & normalize
    
    std::vector<double> p = policy;
    for (size_t i = 0; i < p.size(); ++i) p[i] *= mask[i];
    double sum = std::accumulate(p.begin(), p.end(), 0.0);
    
    if (sum > 1e-8) {
      for (auto& v : p) v /= sum;
    } else {
      sum = 0;
      for(int i=0;i<1575;i++) sum+=mask[i];
      for (size_t i = 0; i < p.size(); ++i)
        p[i] = mask[i] ? (1.0/sum) : 0.0;
    }
    

    for (size_t i = 0; i < p.size(); ++i) {
      if (p[i] > 0.0) {
        children_.emplace_back(new Node(
           /*state*/nullptr, this,
          /*action_taken*/(int)i,
          /*prior*/p[i],
          /*visit_count*/0,
          /*action*/moves[i]
        ));
      }
    }
    
}

  // iterative backpropagation
void Node::backpropagate(double value) {
    Node* node = this;
    while (node) {
      node->value_sum_   += value;
      node->visit_count_ += 1;
      value = -value;
      node  = node->parent_;
    }
}

void Node::delete_nodes() {
    for (auto& c : children_) {
        c->delete_nodes();
    }
    children_.clear();
    if (isExplicitState) {
      delete (state_);
      state_=nullptr;
    }
}

int Node::getStatus() {
    explicitateState();
    return state_->getState();
} 

int  Node::action_taken() const { return action_taken_; }
int  Node::visit_count()  const { return visit_count_; }

