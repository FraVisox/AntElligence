#include <algorithm>
#include "interface.h"

using namespace std;
int evN = 0;

std::pair<double, actionT>
minimax(EBoard* state,
        int depth,
        bool maximizing_player,
        double w[],
        double alpha = -1e9,
        double beta  = +1e9)
{
    // 1) FIRST—check for terminal, no matter what depth is
    int status = checkStatus(state);
    if (status != 1) {
        // status==3/4 → immediate win/loss
        int t = currentTurn(state->graph_board) % 2 == 0;
        if ((status == 3 && t == 0) || (status == 4 && t == 1))
            return { +1000, -1 };
        else
            return { -1000, -1 };
    }

    // 2) THEN, if we’ve hit the depth cutoff, do a static evaluation
    if (depth == 0) {
        evN++;
        return { boardEval(state, w), -1 };
    }

    // 3) Otherwise, proceed with the usual minimax + α-β
    actionT best_action = -1;
    actionT actions[256];
    getActions(state, actions);
    int n = actions[0];

    if (maximizing_player) {
        double max_eval = -1e9;
        for (int i = 1; i <= n; ++i) {
            EBoard* child = copyBoard(state);
            next_state(child, actions[i]);
            auto [evalV, _] = minimax(child, depth-1, false, w, alpha, beta);
            delBoard(child);

            if (evalV > max_eval) {
                max_eval = evalV;
                best_action = actions[i];
            }
            alpha = std::max(alpha, evalV);
            if (beta <= alpha) break;  // β-cut
        }
        return { max_eval, best_action };
    } else {
        double min_eval = +1e9;
        for (int i = 1; i <= n; ++i) {
            EBoard* child = copyBoard(state);
            next_state(child, actions[i]);
            auto [evalV, _] = minimax(child, depth-1, true, w, alpha, beta);
            delBoard(child);

            if (evalV < min_eval) {
                min_eval = evalV;
                best_action = actions[i];
            }
            beta = std::min(beta, evalV);
            if (beta <= alpha) break;  // α-cut
        }
        return { min_eval, best_action };
    }
}


int main(){
    
    double w[]={0,0,0,1,-2,30};
    int Win[2]={0,0};
    for(int Q=0;Q<100;Q++){
        EBoard* state = base_state(0);  // Assume this returns a void* or similar
        int i = 0;

        actionT actions[256];
        int totMov = 0;
    
        while (i < 300) {

            actionT best_action;
            if(i%2==0){
                evN=0;
                pair<double,actionT> rE = minimax(state, 4, true,w);

                double score=rE.first;
                cout<<"Score:: "<<score<<endl;
                best_action=rE.second;
            }else{
                getActions(state,actions);
                int num_actions = actions[0];
                int m=rand()%actions[0]+1;
                best_action=actions[m];              
            }
            // GR.getActions fills `ris` directly
            // Example: GR.getActions(state, ris);
            // If needed, define GR.getActions to match this behavior

            

            cout<<"Best actions with: "<<evN<<" iteration for turn "<<i<<endl;
            //printActionFancy(best_action);
            totMov++;
            next_state(state, best_action);
        
            //printBG(state->board_exp.G);
            i++;

            int out = checkStatus(state);
            if (out != 1) {
                //std::cout << "Stopped at iteration: " << i <<"   Win "<< i%2<<endl;
                Win[i%2]++;
                break;
            }
        }
        delBoard(state);

        cout<<"Win: "<<Win[0]<<"   "<<Win[1]<<endl;
    }

    return 0;

}

