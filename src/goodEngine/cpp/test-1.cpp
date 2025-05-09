#include <algorithm>
#include "interface.h"

using namespace std;
int evN = 0;



std::pair<double, actionT>
minimax(EBoard* state,        int depth,        bool maximizing_player,        double w[],        double alpha = -1e9,        double beta  = +1e9);
int main(){
    actionT actions[256];
    std::pair<double, actionT>  p;
    int totMov = 0; 
    int Win[]={0,0};
    double w1[]={0,0,0,1,-2,30};    
    double w2[]={0,0,0,1,0,30};
    
        
    //pressureOnOpponentQueen
    //pressureOnOwnQueen
    //movableSelf
    //movableOpp
    //nearMyQueen
    //nearOpposite;
    for(int Q=0;Q<10;Q++){
        EBoard* state = base_state(3);  // Assume this returns a void* or similar
        int i = 0;
        int colS=rand()%2;
    
        while (i < 3000) {

            actionT best_action;
            getActions(state,actions);
            evN=0;
            if(i%2==colS){
                p=minimax(state,4,true,w1);
                best_action=p.second;
            }else{
                p=minimax(state,4,true,w2);
                best_action=p.second;
            
            }

            /*for(int i=0;i<num_actions;i++){
                if(stringToAction(state,actionToString(actions[i+1]))!=actions[i+1]){
                    throw "Not the same";
                }
            }*/

            /// If needed, define GR.getActions to match this behavior

            

            cout<<"Best actions with: "<<evN<<" iteration for turn "<<i<<endl;
            //printActionFancy(best_action);
            totMov++;
            next_state(state, best_action);
        
            //printBG(state->board_exp.G);

            int out = checkStatus(state);
            if (out != 1) {
                //std::cout << "Stopped at iteration: " << i <<"   Win "<< i%2<<endl;
                if(colS==i%2)
                    Win[0]++;
                else
                    Win[1]++;
                break;
            }
            i++;
        }
        delBoard(state);

        cout<<"Win: "<<Win[0]<<"   "<<Win[1]<<endl;
    }

    return 0;

}


std::pair<double, actionT>
minimax(EBoard* state,
        int depth,
        bool maximizing_player,
        double w[],
        double alpha ,
        double beta  )
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
