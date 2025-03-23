#include <torch/torch.h> //Needs libtorch
#include <iostream>

// Game class (simplified)
class Game {
public:
    int row_count;
    int column_count;
    int action_size;

    Game(int rows, int cols, int actions) : row_count(rows), column_count(cols), action_size(actions) {}
};

// ResBlock implementation
class ResBlock : public torch::nn::Module {
public:
    ResBlock(int num_hidden) {
        conv1 = register_module("conv1", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(num_hidden, num_hidden, 3).padding(1)));
        bn1 = register_module("bn1", torch::nn::BatchNorm2d(num_hidden));
        conv2 = register_module("conv2", torch::nn::Conv2d(
            torch::nn::Conv2dOptions(num_hidden, num_hidden, 3).padding(1)));
        bn2 = register_module("bn2", torch::nn::BatchNorm2d(num_hidden));
    }

    torch::Tensor forward(torch::Tensor x) {
        torch::Tensor residual = x;
        x = torch::relu(bn1->forward(conv1->forward(x)));
        x = bn2->forward(conv2->forward(x));
        x += residual;
        x = torch::relu(x);
        return x;
    }

private:
    torch::nn::Conv2d conv1{nullptr}, conv2{nullptr};
    torch::nn::BatchNorm2d bn1{nullptr}, bn2{nullptr};
};

// ResNet implementation
class ResNet : public torch::nn::Module {
public:
    ResNet(const Game& game, int num_resBlocks, int num_hidden, torch::Device device) 
        : device_(device) {
        
        // Start block
        startBlock = register_module("startBlock", torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(3, num_hidden, 3).padding(1)),
            torch::nn::BatchNorm2d(num_hidden),
            torch::nn::ReLU()
        ));
        
        // Backbone
        for (int i = 0; i < num_resBlocks; i++) {
            backBone->push_back(ResBlock(num_hidden));
        }
        register_module("backBone", backBone);
        
        // Policy head
        policyHead = register_module("policyHead", torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, 32, 3).padding(1)),
            torch::nn::BatchNorm2d(32),
            torch::nn::ReLU(),
            torch::nn::Flatten(),
            torch::nn::Linear(32 * game.row_count * game.column_count, game.action_size)
        ));
        
        // Value head
        valueHead = register_module("valueHead", torch::nn::Sequential(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(num_hidden, 3, 3).padding(1)),
            torch::nn::BatchNorm2d(3),
            torch::nn::ReLU(),
            torch::nn::Flatten(),
            torch::nn::Linear(3 * game.row_count * game.column_count, 1),
            torch::nn::Tanh()
        ));
        
        // Move model to specified device
        to(device_);
    }

    std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor x) {
        x = startBlock->forward(x);
        for (auto& module : *backBone) {
            auto resBlock = std::dynamic_pointer_cast<ResBlock>(module);
            x = resBlock->forward(x);
        }
        
        torch::Tensor policy = policyHead->forward(x);
        torch::Tensor value = valueHead->forward(x);
        
        return {policy, value};
    }

private:
    torch::Device device_;
    torch::nn::Sequential startBlock{nullptr};
    torch::nn::ModuleList backBone = register_module("backBone", torch::nn::ModuleList());
    torch::nn::Sequential policyHead{nullptr}, valueHead{nullptr};
};

// Example usage
int main() {
    // Create a game instance
    Game game(8, 8, 64); // 8x8 board with 64 possible actions
    
    // Initialize model
    auto device = torch::kCPU;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    
    ResNet model(game, 19, 256, device);
    
    // Create a sample input
    torch::Tensor input = torch::randn({1, 3, game.row_count, game.column_count}).to(device);
    
    // Forward pass
    auto [policy, value] = model.forward(input);
    
    std::cout << "Policy output shape: " << policy.sizes() << std::endl;
    std::cout << "Value output shape: " << value.sizes() << std::endl;
    
    return 0;
}