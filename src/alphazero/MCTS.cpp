#include <torch/torch.h> //Requires Libtorch
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <limits>
#include <memory>

// Forward declaration of Game class
class Game {
public:
    int action_size;
    
    std::vector<float> get_encoded_state(const std::vector<std::vector<int>>& state) {
        // Implementation depends on your game
        return std::vector<float>();
    }
    
    std::vector<std::vector<int>> get_next_state(const std::vector<std::vector<int>>& state, int action, int player) {
        // Implementation depends on your game
        return std::vector<std::vector<int>>();
    }
    
    std::vector<std::vector<int>> change_perspective(const std::vector<std::vector<int>>& state, int player) {
        // Implementation depends on your game
        return std::vector<std::vector<int>>();
    }
    
    float get_opponent_value(float value) {
        return -value;
    }
    
    std::pair<float, bool> get_value_and_terminated(const std::vector<std::vector<int>>& state, int action) {
        // Implementation depends on your game
        return {0.0f, false};
    }
    
    std::vector<int> get_valid_moves(const std::vector<std::vector<int>>& state) {
        // Implementation depends on your game
        return std::vector<int>(action_size, 1);
    }
};

// Node class
class Node {
public:
    Game* game;
    std::map<std::string, float> args;
    std::vector<std::vector<int>> state;
    Node* parent;
    int action_taken;
    float prior;
    
    std::vector<std::unique_ptr<Node>> children;
    
    int visit_count;
    float value_sum;
    
    Node(Game* game, const std::map<std::string, float>& args, 
         const std::vector<std::vector<int>>& state, 
         Node* parent = nullptr, int action_taken = -1, 
         float prior = 0.0f, int visit_count = 0)
        : game(game), args(args), state(state), parent(parent), 
          action_taken(action_taken), prior(prior), visit_count(visit_count), value_sum(0.0f) {}
    
    bool is_fully_expanded() const {
        return !children.empty();
    }
    
    Node* select() {
        Node* best_child = nullptr;
        float best_ucb = -std::numeric_limits<float>::infinity();
        
        for (const auto& child : children) {
            float ucb = get_ucb(child.get());
            if (ucb > best_ucb) {
                best_child = child.get();
                best_ucb = ucb;
            }
        }
        
        return best_child;
    }
    
    float get_ucb(Node* child) const {
        float q_value = 0.0f;
        if (child->visit_count > 0) {
            q_value = 1.0f - ((child->value_sum / child->visit_count) + 1.0f) / 2.0f;
        }
        
        return q_value + args.at("C") * (std::sqrt(float(visit_count)) / (child->visit_count + 1)) * child->prior;
    }
    
    Node* expand(const std::vector<float>& policy) {
        Node* last_child = nullptr;
        
        for (int action = 0; action < int(policy.size()); ++action) {
            if (policy[action] > 0) {
                auto child_state = state; // Copy state
                child_state = game->get_next_state(child_state, action, 1);
                child_state = game->change_perspective(child_state, -1);
                
                auto child = std::make_unique<Node>(game, args, child_state, this, action, policy[action]);
                last_child = child.get();
                children.push_back(std::move(child));
            }
        }
        
        return last_child;
    }
    
    void backpropagate(float value) {
        value_sum += value;
        visit_count += 1;
        
        value = game->get_opponent_value(value);
        if (parent != nullptr) {
            parent->backpropagate(value);
        }
    }
};

// MCTS class
class MCTS {
public:
    Game* game;
    std::map<std::string, float> args;
    std::shared_ptr<ResNet> model;
    
    MCTS(Game* game, const std::map<std::string, float>& args, std::shared_ptr<ResNet> model)
        : game(game), args(args), model(model) {}
    
    std::vector<float> search(const std::vector<std::vector<int>>& state) {
        // Create root node
        auto root = std::make_unique<Node>(game, args, state, nullptr, -1, 0.0f, 1);
        
        // Get policy from model
        torch::NoGradGuard no_grad;
        
        auto encoded_state = game->get_encoded_state(state);
        torch::Tensor state_tensor = torch::from_blob(encoded_state.data(), 
                                                     {1, 3, int(state.size()), int(state[0].size())},
                                                     torch::kFloat).to(model->device);
        
        auto [policy, _] = model->forward(state_tensor);
        
        auto policy_prob = torch::softmax(policy, 1).squeeze(0).cpu();
        std::vector<float> policy_np(policy_prob.data_ptr<float>(), 
                                     policy_prob.data_ptr<float>() + game->action_size);
        
        // Apply Dirichlet noise
        std::random_device rd;
        std::mt19937 gen(rd());
        std::gamma_distribution<float> gamma(args["dirichlet_alpha"], 1.0f);
        
        std::vector<float> dirichlet_noise(game->action_size);
        float noise_sum = 0.0f;
        for (int i = 0; i < game->action_size; ++i) {
            dirichlet_noise[i] = gamma(gen);
            noise_sum += dirichlet_noise[i];
        }
        
        // Normalize the noise
        for (int i = 0; i < game->action_size; ++i) {
            dirichlet_noise[i] /= noise_sum;
            policy_np[i] = (1 - args["dirichlet_epsilon"]) * policy_np[i] + 
                            args["dirichlet_epsilon"] * dirichlet_noise[i];
        }
        
        // Apply valid moves mask
        auto valid_moves = game->get_valid_moves(state);
        float policy_sum = 0.0f;
        for (int i = 0; i < game->action_size; ++i) {
            policy_np[i] *= valid_moves[i];
            policy_sum += policy_np[i];
        }
        
        // Normalize
        for (int i = 0; i < game->action_size; ++i) {
            policy_np[i] /= policy_sum;
        }
        
        // Expand root
        root->expand(policy_np);
        
        // Perform MCTS searches
        for (int search = 0; search < int(args["num_searches"]); ++search) {
            Node* node = root.get();
            
            // Selection
            while (node->is_fully_expanded()) {
                node = node->select();
            }
            
            // Evaluation
            auto [value, is_terminal] = game->get_value_and_terminated(node->state, node->action_taken);
            value = game->get_opponent_value(value);
            
            if (!is_terminal) {
                // Get policy and value from neural network
                auto encoded_node_state = game->get_encoded_state(node->state);
                torch::Tensor node_state_tensor = torch::from_blob(
                    encoded_node_state.data(),
                    {1, 3, int(node->state.size()), int(node->state[0].size())},
                    torch::kFloat).to(model->device);
                
                auto [node_policy, node_value] = model->forward(node_state_tensor);
                
                auto node_policy_prob = torch::softmax(node_policy, 1).squeeze(0).cpu();
                std::vector<float> node_policy_np(node_policy_prob.data_ptr<float>(), 
                                              node_policy_prob.data_ptr<float>() + game->action_size);
                
                // Apply valid moves mask
                auto node_valid_moves = game->get_valid_moves(node->state);
                float node_policy_sum = 0.0f;
                for (int i = 0; i < game->action_size; ++i) {
                    node_policy_np[i] *= node_valid_moves[i];
                    node_policy_sum += node_policy_np[i];
                }
                
                // Normalize
                for (int i = 0; i < game->action_size; ++i) {
                    node_policy_np[i] /= node_policy_sum;
                }
                
                value = node_value.item<float>();
                
                // Expansion
                node->expand(node_policy_np);
            }
            
            // Backpropagation
            node->backpropagate(value);
        }
        
        // Calculate action probabilities
        std::vector<float> action_probs(game->action_size, 0.0f);
        for (const auto& child : root->children) {
            action_probs[child->action_taken] = float(child->visit_count);
        }
        
        // Normalize
        float sum_visits = 0.0f;
        for (float prob : action_probs) {
            sum_visits += prob;
        }
        
        for (int i = 0; i < game->action_size; ++i) {
            action_probs[i] /= sum_visits;
        }
        
        return action_probs;
    }
};