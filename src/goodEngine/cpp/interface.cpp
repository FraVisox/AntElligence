
#include <math.h>
#include <iostream>

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


actionT stringToAction(EBoard* b,char* str){
    pieceT startP,destP;
    direction dir ;

    if(str[0]=='p')return pass();
    
    startP=decodeBug(str);



    int q=1; while(str[q]!='w' && str[q]!='b' && str[q]!=0) q++;

    if(str[q]==0) return placeFirst(startP);
    
    destP=decodeBug(str+q);

    q=2;
    if(str[q]=='1' || str[q]=='2' || str[q]=='3') q++;

    while(str[q]==' ') q++;

    if(str[q]=='w' || str[q]=='b'){  // is the case BUG BUG dir;
        q+=2; if(str[q]>'0' && str[q]<'4')q++;
        switch(str[q]){
            case '/': dir=5;break;
            case '-': dir=0;break;
            case '\\': dir=1;break;
            case 0: dir=7;break;
            default : throw "WTF??";
        }
    }else{
        if(str[q]>'0' && str[q]<'4')q++;
        switch(str[q]){
            case '/': dir=2;break;
            case '-': dir=3;break;
            case '\\': dir=4;break;
            default: throw "NOT A VALID STRING";
        }
    }
    //cout<<"Conver"<< str<<" to :"<<0+startP<<","<<0+destP<<" "<<dir<<endl;
    position destPos=b->board_exp.G.getPosition(destP).applayMove(dir);
    return movement(startP,destPos ,b->board_exp.G);
}

char* BoardRapp(EBoard* p){
    return p->graph_board;
}





EBoard* base_state(int gt){
    cout<<"GT:"<<gt<<endl;
    return new EBoard((GameType)gt);
}

EBoard* copyBoard(EBoard* b){
    return new EBoard(b);
}

void next_state(EBoard* state,actionT action){
    state->applyAction(action);
}

void getActions(EBoard* state,actionT* actions){  // max 256 mosse
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
    a&=((1ull<<63)-1);
    if(a==0){
        char* r=(char*)malloc(sizeof(char)*5);
        r[0]='p';
        r[1]='a';
        r[2]='s';
        r[3]='s';
        r[4]=0;
        return r;
    }
    char b=a&0xff;
    string s=PiecetoString(b);
    for(int i=7;i>=1;i--){
        char can=((a>>(i*8))&0xff);
        if(can!=0){
            s+=" "+nameDirToString(PiecetoString(can),opposite(i-1));
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

    
double boardEval(EBoard* b, double w[]){
    PlayerColor MyCol=b->board_exp.currentColor();
    pieceT myQueen=(MyCol==PlayerColor::WHITE)?8:22;
    pieceT oppositeQueen=30-myQueen;
    

    int nearMyQueen=0;
    int nearOpposite=0;
    int sM=getStartingPointBug(myQueen);
    int sO=getStartingPointBug(oppositeQueen);
    for(int i=0;i<6;i++){
        if(b->graph_board[sM+i]!=0){
            nearMyQueen++;
        }
        if(b->graph_board[sO+i]!=0){
            nearOpposite++;
        }
    }

    int pressureOnOwnQueen=(b->graph_board[sM+7]!=0)?1:0;
    int pressureOnOpponentQueen=(b->graph_board[sO+7]!=0)?1:0;

    int movableSelf=0, movableOpp=0;
    int unplacedMy=0, unplacedOpp=0;
    int t=currentTurn(b->graph_board);
    for(int p=1;p<=28;p++){
        if(isPlaced(b->graph_board,p)&& b->board_exp.G.canMoveWithoutBreakingHiveRule(p,t+1))
        {
            if(col(p)==MyCol)movableSelf++;
            else movableOpp++;
        }else if(isInHand(b->graph_board,p)){
            
            if(col(p)==MyCol)unplacedMy++;
            else     unplacedOpp++;
        }
    }

    double score = 0.0;
    
    score += w[0] * pressureOnOpponentQueen;
    score += w[1] * pressureOnOwnQueen;
    score += w[2] * movableSelf;
    score += w[3] * movableOpp ;
    score += w[4] * nearMyQueen;
    score += w[5] * nearOpposite;
    
    return score;
}


void delBoard(EBoard* b){
    free(b);
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
