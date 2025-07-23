#include "cpp_minimax/engine.h"
#include "cpp_minimax/cpp/interface_reduced.cpp"
#include <string>
using namespace std;

void Engine::newgame(string arguments){
   board = base_state(7);
}

void Engine::validmoves(){
    getActions(board, actions);
    // TODO: return them
}

void Engine::bestmove(string param){
    cout << agent.calculate_best_move(board)<<"\n";
}

void Engine::play(string move){
    next_state(board, 1); //TODO: pass the move

    printBoard(board);
}