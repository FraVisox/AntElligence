#include "engine.h"
#include "cpp_minimax/cpp/interface_reduced.cpp"
#include <string>
using namespace std;

void Engine::newgame(string arguments){
   board = base_state(7);
}
   

void Engine::validmoves(){

    vector<action> moves=board.possibleMoves();
    if(moves.size()==0){
        cout<<"pass\n";
    }

    for(int i=0;i<moves.size()-1;i++){
        cout<<ActionToString(moves[i])<<";";
    }
    cout<<ActionToString(moves[moves.size()-1])<<"\n";
}
void Engine::bestmove(string param){
    //TODO: see what is the parameter of the string and pass it to the agent
    cout << (param);
    cout << "Param\n";
    cout << ActionToString(agent.calculate_best_move(board))<<"\n";
}

void Engine::play(string move){
    board.executeAction(move);

    cout<<board.toString()<<"\n";
}

void Engine::undo(string arguments="0"){
    board.undo(stoi(arguments));
    cout<<board.toString()<<"\n";
}