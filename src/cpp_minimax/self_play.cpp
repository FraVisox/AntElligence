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

    MinimaxAgent a(20);

    //try{
    srand(time(0));

    Board board(GameType::Base_MLP);
    board.applayAction(stringToAction(board,"wB1"));
    board.applayAction(stringToAction(board,"bS1 -wB1"));
int i=0;
    while(board.currentTurn<100 && board.getGameState()<2){
        i++;
        actionT M;
        if(board.currentTurn&1){
            board.ComputePossibleMoves();
            M=board.resAction[rand()%board.numAction];
        }else
            M=a.calculate_best_move(board);
        cout<<i<<actionToString(M,board)<<endl;
        board.applayAction(M);
    }
    return 0;
}