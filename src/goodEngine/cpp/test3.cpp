#include "interface.h"
#include <torch/torch.h>

int main(){

    EBoard* b=base_state(0);
    char s[16]="model_0-mcts.pt";
    auto test = torch::jit::load("model_0-mcts.pt");
    
    MCTS* mcts=(MCTS*)createMCTS(s);
    
    std::cout 
        << mcts->model_.get_method("forward")
            .function()
            .getSchema()
        << std::endl;


    vector<double> vp=mcts->search(b);
    for(int i=0;i<1575;i++){
        cout<<vp[i]<<" ";
    }
    cout<<endl;
}