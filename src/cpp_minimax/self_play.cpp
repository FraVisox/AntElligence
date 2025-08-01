#include <iostream>


#include "engine.h"
#include "minimax.h"
#include "minimax_tt.h"
#include "minimax.h"
/*
#include "engine/action.cpp"
#include "engine/board_bitset.cpp"
#include "engine/board.cpp"
#include "engine/board_score.cpp"
#include "engine/board_score_mzinga.cpp"
#include "engine/direction.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "minimax.cpp"
#include "minimax_tt.cpp"
#include "engine.cpp"
*/

int main(){

    MinimaxAgent a(5,10);

    Engine e(a);
    try{

    e.board= Board(Base_MLP);
    e.board.applayAction(stringToAction(e.board,"wB1"));
    e.board.applayAction(stringToAction(e.board,"bS1 -wB1"));

    while(e.board.currentTurn<10){
        actionT M=a.calculate_best_move(e.board);
        cout<<actionToString(M,e.board)<<endl;
        e.board.applayAction(M);
    }
}catch(char* e){
    cout<<"NO"<<endl;
    cout<<e<<endl;
}
    return 0;
}