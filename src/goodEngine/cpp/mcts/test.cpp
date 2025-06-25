#include <torch/script.h>   // for torch::jit::Module
#include <torch/cuda.h>   // defines torch::cuda::is_available()
#include <iostream>
#include <memory>
#include "mcts.h"
int main(int argc, char* argv[]) {
    // 1) Path to your serialized model
    const std::string model_path = "model.pt";

    // 2) Load the module
    std::shared_ptr<torch::jit::script::Module> model;
    try {
        model = std::make_shared<torch::jit::script::Module>(torch::jit::load(model_path));
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model from " << model_path << "\n"
                  << e.what() << std::endl;
        return -1;
    }

    // 3) (Optional) Print its methods/graph
    std::cout << "Successfully loaded model:\n";
    model->dump(true, true, true);  // prints method names and graph IR

    // 4) Move to GPU/CPU as needed
    auto device = torch::kCUDA;
    if (!torch::cuda::is_available()) {
      std::cout << "CUDA not available, falling back to CPU\n";
      device = torch::kCPU;
    }

    MCTS mcts(model);

    
    return 0;
}
