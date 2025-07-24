#include "cpp_minimax/engine.h"
#include "cpp_minimax/cpp/engine/enums.h"
#include "cpp_minimax/cpp/engine/board.h"
#include "cpp_minimax/cpp/interface_reduced.cpp"
#include <string>
using namespace std;

void Engine::newgame(string s){
    board = Board(Base_MLP);
    //cout << board.toString() << "\n"; TODO: to string
}
   

void Engine::validmoves(){
    if (board.getGameState() == NOT_STARTED)
        error("Game has not started yet");
    else if (board.getGameState() != STARTED)
        error("The game is over");

    board.ComputePossibleMoves();

    ;
    ;

    if(board.numAction==0){
        cout<<"pass\n";
    }

    for(int i=0;i<board.numAction-1;i++){
        cout<<actionToString(board.resAction[i], board)<<";";
    }
    cout<<actionToString(board.resAction[board.numAction-1], board)<<"\n";
}

void Engine::bestmove(string param){
    /*
    if (param.find("depth")) {
        agent.set_depth_limit(atoi(param.substr(6).c_str()));
    } else if (param.find("time")) {
        //agent.set_time_limit(atoi(param.substr(6).c_str()));
    } else {
        error("Invalid command");
    }
        */
    cout << actionToString(agent.calculate_best_move(board), board)<<"\n";
}

void Engine::play(string move){
    board.applayAction(stringToAction(board, move));
    //cout<<board.toString()<<"\n";
}