
#include <math.h>
#include <iostream>
#include <regex>

#ifndef INTERFACE_CPP
#define INTERFACE_CPP
#include "graph_board.h"
#include "graph_board.h"
#include "build_from_graph.h"


/*#include "engine/action.cpp"
#include "engine/board.cpp"
#include "engine/direction.cpp"
#include "engine/enums.cpp"
#include "engine/gameboard.cpp"
#include "engine/piece.cpp"
#include "engine/position.cpp"
#include "graph_board.cpp"
#include "build_from_graph.cpp"*/  

// Define export macro for Windows
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif



char voidBoard[282];

void next_state(char* ris,char* state,actionT action){
    findNextState(ris,state,action);
}

int getActions(char* state,int64_t* actions){  // max 512 mosse
    return getActionsWithExplicitTransiction(state,actions);
}


char* baseGame(){
    char* ris=(char*)calloc(BOARDSIZE,sizeof(char));
        
    setTurn(ris,1);
    for(int i=1;i<12;i++){
        addPieceInHand(ris,i);
        addPieceInHand(ris,i+14);
    }
    return ris;
}


char checkStatus(char* board){
    return checkWin(board);
}




/*
    OK = 0
    INVALID_GAME_NOT_STARTED = 1
    GAME_OVER_WHITE_WINS = 2
    GAME_OVER_BLACK_WINS = 3
    GAME_OVER_DRAW = 2
    
    
*/

/* BRIEF DESCRIPTION OF THE STRINGS FORMAT

- GAMESTRING: no trailing semicolon at the end of the string

GameTypeString;GameStateString;TurnString;MoveString1;MoveString2;MoveStringN

- GAMETYPESTRING:

Base
Base+M
Base+L
Base+P
Base+ML
Base+MP
Base+LP
Base+MLP

- GAMESTATESTRING:

NotStarted	The game has not started, no moves have been made.
InProgress	The game is in progress, at least one move has been made.
Draw	    The game is over, the result is a draw.
WhiteWins	The game is over, the white side has won.
BlackWins	The game is over, the black side has won.

- TURNSTRING:

White[1]
Black[1]

- MOVESTRING: 

The string starts with the piece to move's short name, followed by a space, 
followed by a position relative to another piece on the board. 
The piece's short name is simply the piece's color w for white or b for 
black, followed by a single letter representation of the piece's bug type, 
and a number when a player has multiple bugs of that type, indicating the 
order in which the piece was initially played.

wQ / bQ
wS1 / bS1
wS2 / bS2
wB1 / bB1
wB2 / bB2
wG1 / bG1
wG2 / bG2
wG3 / bG3
wA1 / bA1
wA2 / bA2
wA3 / bA3
wM / bM
wL / bL
wP / bP

For the first move of the game, simply state the piece being played.

For moving pieces on top of the hive (as the beetle, or the mosquito adopting 
the Beetle's movement), simply state the target piece that is about to be covered.

A passing move (made because a side has no other moves) is simply pass

*/
#endif