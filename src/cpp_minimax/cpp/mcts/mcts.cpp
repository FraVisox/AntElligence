
// -----------------------------------------------------------------------------
// MCTS orchestration
#include "node.h"
#include "args.h"
#include "mcts.h"


  // returns a vector of length MAX_ACTIONS
std::vector<double> MCTS::search(EBoard* root_state,
                             int turn){
                          
    cout<<"Start search"<<endl;
    // 1) build root
    Node root(root_state, /*parent*/nullptr, /*action_taken*/-1, /*prior*/0.0, /*visit_count*/1);
    auto [bm, bmeta] = root.getVectSplit();
    // 2) initial eval;
    
    torch::Tensor input_m = bm.unsqueeze(0).to(device_,true);
    torch::Tensor input_t = bmeta.unsqueeze(0).to(device_,true);
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input_m);
    inputs.push_back(input_t);

    
    torch::NoGradGuard no_grad;

    auto out=model_.forward(inputs).toTuple();
    auto policy_t = out->elements()[0].toTensor();
    torch::Tensor policy = torch::softmax(policy_t, /*dim=*/1)
                             .squeeze(0)
                             .cpu();

    std::vector<double> policy_v(policy.data_ptr<float>(),
                                 policy.data_ptr<float>() + policy.numel());

    // Dirichlet noise
    if (turn <= ARG_dirichlet_turn) {
      std::gamma_distribution<double> g(ARG_dirichlet_alpha, 1.0);
      std::mt19937_64 gen{std::random_device{}()};
      std::vector<double> noise(policy_v.size());
      for (auto& x : noise) x = g(gen);
      double sum_n = std::accumulate(noise.begin(), noise.end(), 0.0);
      for (size_t i = 0; i < noise.size(); ++i)
        policy_v[i] = (1 - ARG_dirichlet_epsilon) * policy_v[i]
                    +  ARG_dirichlet_epsilon * (noise[i] / sum_n);
    }

    // 3) expand root
    root.expand(policy_v);
    // 4) main loop
    int64_t searched = 0;

    while (searched < ARG_num_searches) {
      // select a batch
      auto batch_nodes = root.selectMultiple(ARG_batch_size);

      cout<<"Start for batch with "<<batch_nodes.size()<<" Nodes"<<endl;
      // gather evaluations
      std::vector<torch::Tensor> mats, metas;
      mats.reserve(batch_nodes.size());
      metas.reserve(batch_nodes.size());

      for (auto* node : batch_nodes) {
        int status = node->getStatus();
        if (status == 0|| status ==1) {
          auto [m, t] = node->getVectSplit();
          mats.push_back(m);
          metas.push_back(t);
        } else {
          double v = (status == 2? 1.0
                   : (status == 3? -1.0 : 0.0));
          node->backpropagate(v);
        }
      }

      searched += (int)mats.size();
      if (mats.empty()) break;

      // batch-eval
      auto xb = torch::stack(mats).to(device_, /*non_blocking=*/true);
      auto xt = torch::stack(metas).to(device_, /*non_blocking=*/true);

       std::vector<torch::jit::IValue> inputs;
            inputs.push_back(xb);
            inputs.push_back(xt);
      auto out2 = model_.forward(inputs).toTuple();

      auto pols_t = out2->elements()[0].toTensor();
      auto vals_t = out2->elements()[1].toTensor();
      auto pols = torch::softmax(pols_t, /*dim=*/1).cpu();
      auto vals = vals_t.cpu().squeeze(-1);

      // apply results
      auto pols_data = pols.accessor<float,2>();
      auto vals_data = vals.accessor<float,1>();
      for (size_t i = 0; i < batch_nodes.size(); ++i) {
        std::vector<double> pv(pols.size(1));
        for (int j = 0; j < pols.size(1); ++j)
          pv[j] = pols_data[i][j];
        batch_nodes[i]->expand(pv);
        batch_nodes[i]->backpropagate(vals_data[i]);
      }
    }

    // 5) collect final policy (visit counts)
    std::vector<double> final_probs(MAX_ACTIONS, 0.0);
    for (auto& c : root.children_) {
      final_probs[c->action_taken()] = (double)c->visit_count();
    }
    double s = std::accumulate(final_probs.begin(), final_probs.end(), 0.0);
    if (s > 0) for (auto& x : final_probs) x /= s;

    // 6) cleanup
    for(auto& c : root.children_) c->delete_nodes();
    //root.delete_nodes();
    return final_probs;
}



MCTS::MCTS(const std::string& model_path):device_(at::kCPU)
{
    device_ = torch::cuda::is_available() ? at::kCUDA : at::kCPU;
    try {
        // Load the serialized TorchScript module from file
        model_ = torch::jit::load(model_path);  
        
        model_.to(device_);
        model_.eval();  
        // Optionally, move to the desired device:
        // model_->eval();    // if you want to disable dropout/batchnorm training behavior
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading TorchScript model from '"
                  << model_path << "':\n  " << e.what() << std::endl;
        throw;  // rethrow if you want the caller to handle this failure
    }
}
