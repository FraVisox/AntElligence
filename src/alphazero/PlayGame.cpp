#include <iostream>
#include <algorithm>
#include <string>
#include <memory>


//We are assuming that everything else is initalized already


int main() {
    // Initialize the game
    ConnectFour game;
    int player = 1;
    
    // Define MCTS parameters
    std::map<std::string, float> args = {
        {"C", 2.0f},
        {"num_searches", 600.0f},
        {"dirichlet_epsilon", 0.0f},
        {"dirichlet_alpha", 0.3f}
    };
    
    // Set up device (CPU or GPU)
    torch::DeviceType device_type = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Using GPU." << std::endl;
        device_type = torch::kCUDA;
    }
    torch::Device device(device_type);
    
    // Initialize the model
    auto model = std::make_shared<ResNet>(game, 9, 128, device);
    
    // Load pre-trained model weights
    try {
        torch::load(model->parameters(), "model_7_ConnectFour.pt");
        std::cout << "Model loaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return 1;
    }
    
    // Set model to evaluation mode
    model->eval();
    
    // Initialize MCTS
    MCTS mcts(&game, args, model);
    
    // Get initial state
    auto state = game.get_initial_state();
    
    // Main game loop
    while (true) {
        // Print current state
        game.print_state(state);
        
        int action;
        
        if (player == 1) {
            // Human player's turn
            auto valid_moves = game.get_valid_moves(state);
            
            // Print valid moves
            std::cout << "Valid moves: ";
            for (int i = 0; i < game.action_size; i++) {
                if (valid_moves[i] == 1) {
                    std::cout << i << " ";
                }
            }
            std::cout << std::endl;
            
            // Get player input
            std::cout << player << ": ";
            std::string input;
            std::getline(std::cin, input);
            try {
                action = std::stoi(input);
            } catch (const std::exception& e) {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                continue;
            }
            
            // Check if the move is valid
            if (action < 0 || action >= game.action_size || valid_moves[action] == 0) {
                std::cout << "Action not valid" << std::endl;
                continue;
            }
        } else {
            // AI player's turn
            auto neutral_state = game.change_perspective(state, player);
            auto mcts_probs = mcts.search(neutral_state);
            
            // Find the move with the highest probability
            auto max_element = std::max_element(mcts_probs.begin(), mcts_probs.end());
            action = std::distance(mcts_probs.begin(), max_element);
            
            std::cout << "AI plays column " << action << std::endl;
        }
        
        // Update the game state
        state = game.get_next_state(state, action, player);
        
        // Check for win or draw
        auto [value, is_terminal] = game.get_value_and_terminated(state, action);
        
        if (is_terminal) {
            game.print_state(state);
            if (value == 1) {
                std::cout << "Player " << player << " won!" << std::endl;
            } else {
                std::cout << "Draw!" << std::endl;
            }
            break;
        }
        
        // Switch to the other player
        player = game.get_opponent(player);
    }
    
    return 0;
}