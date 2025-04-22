
#include <math.h>
#include <iostream>
#include <regex>

#ifndef INTERFACE_CPP
#define INTERFACE_CPP
#include "embadded_board.h"

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


EBoard* base_state(int gt){
    return new EBoard((GameType)gt);
}

EBoard* copyBoard(EBoard* b){
    return new EBoard(b);
}

void next_state(EBoard* state,actionT action){
    state->applyAction(action);
}

void getActions(EBoard* state,int64_t* actions){  // max 256 mosse
    state->getNextsActions(actions);    
}


int checkStatus(EBoard* board){
    return board->getState();
}

char* toVectorNear(EBoard* b){
    return b->graph_board;
}

void PrintBoard(EBoard* b){
    printBoardFancy(b->graph_board);
}


char* actionToString(actionT a){
    char b=a&0xff;
    string s=PiecetoString(b);
    for(int i=1;i<=7;i++){
        char can=((a>>(i*8))&0xff);
        if(can!=0){
            s+=" "+nameDirToString(PiecetoString(can),allDirections[opposite(i-1)]);
            break;
        }
    }
    char* r=(char*)malloc(sizeof(char)*s.size()+1);
    for(int i=0;i<s.size();i++){
        r[i]=s[i];
    }
    r[s.size()]=0;
    return r;
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