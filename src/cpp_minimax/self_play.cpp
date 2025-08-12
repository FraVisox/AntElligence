#include <iostream>


#include "engine.h"
#include "minimax.h"
//#include "minimax_tt.h"
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

    MinimaxAgent a(5);

    //try{
    srand(51);

    Board board(GameType::Base_MLP);
    int i=0;
    while(board.currentTurn<80 && board.getGameState()<2){
        i++;
        actionT M;
        if(board.currentTurn&1 ||board.currentTurn<30 ){
            board.ComputePossibleMoves();
            M=board.resAction[rand()%board.numAction];
        }else{
            M=a.calculate_best_move(board);
        }
        cout<<i<<actionToString(M,board)<<endl;
        board.applayAction(M);
    }
    cout<<"Game ended with status"<<board.getGameState()<<endl;
    return 0;
}
