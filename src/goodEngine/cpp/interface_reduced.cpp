
#include <math.h>
#include <iostream>

#ifndef INTERFACE_CPP
#define INTERFACE_CPP
#include "engine/embadded_board.h"

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif


void updatePossiblemoves(EBoard* state);

actionT stringToAction(EBoard* b,char* str){
    cout<<"Start string_to_action"<<endl;
    /*for(int i=0;str[i]!=0;i++){
        if(str[i]>='A' && str[i]<='Z')
            str[i]+=32;
    }
    cout<<endl;
    */
    pieceT startP,destP;
    direction dir ;

    if(str[0]=='p')return pass();
    
    startP=decodeBug(str);


    int q=1; 
    while(str[q]!='w' && str[q]!='b' && str[q]!=0) q++;

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
            case 0: dir=6;break;
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
    positionT destPos=applayMove(b->board_exp.G.getPosition(destP),dir);
    return movement(startP,destPos);
}


void* getMask(EBoard* p){
    if(!p->isUpdated){
        updatePossiblemoves(p);
    }
    return p->board_exp.G.isValidMoveBitmask;

}


void* getAssociatedAction(EBoard* p){
    if(!p->isUpdated){
        updatePossiblemoves(p);
    }
    return p->board_exp.G.associatedAction;
}


void updatePossiblemoves(EBoard* state){
    state->board_exp.ComputePossibleMoves();
    state->isUpdated=true;
}

EBoard* base_state(int gt){
    return new EBoard((GameType)gt);
}

EBoard* copyBoard(EBoard* b){
    return new EBoard(b);
}

void next_state(EBoard* state,actionT action){
    state->applyAction(action);
    state->isUpdated=false;
}

void getActions(EBoard* state,actionT* actions){  // max 256 mosse
    state->getNextsActions(actions);   
    state->isUpdated=true;
}

int checkStatus(EBoard* board){
    return board->getState();
}


void* getStatusVector(EBoard* board){
    board->updateVectRapp();
    
    return board->vectRapp;
}

void printBoard(EBoard* board){
    for(int i=16;i<48;i++){
        printf("%4d",(32*i+16)&1023);
        if(i<10)cout<<" ";
        for(int j=16;j<i;j++)std::cout<<"  ";
        for(int j=16;j<48;j++){
            gameboard &g=board->board_exp.G;
            if(g.high[(j+32*i)&1023]==0){
                cout<<"   ";
            }
            if(g.high[(j+32*i)&1023]>1){
                cout<<"XXX";
            }
            if(g.high[(j+32*i)&1023]==1){
                pieceT p=g.topPiece((j+32*i)&1023);
                string s=PiecetoString(p);
                cout<<s;
                if(s.size()==2)cout<<" ";
            }            
            cout<<"|";
        }
        cout<<endl;
    }
}



void actionToString(actionT a, EBoard *board,char* risBug){
    if(a==0){
        risBug[0]='p';
        risBug[1]='a';
        risBug[2]='s';
        risBug[3]='s';
        risBug[4]=0;
        return;
    }
    
    pieceT bug=a&31;
    positionT pos=a>>5;

    string s=PiecetoString(bug);
    gameboard& g=board->board_exp.G;
    if(!g.isFree(pos)){
        pieceT resPiece=board->board_exp.G.topPiece(pos);   
        s+=" "+nameDirToString(PiecetoString(resPiece),opposite(6));  

    }else{
        for(int dir=0;dir<6;dir++){
            positionT next=applayMove(pos,dir);
            if(!g.isFree(next)){
                pieceT resPiece=board->board_exp.G.topPiece(next);
                s+=" "+nameDirToString(PiecetoString(resPiece),opposite(dir));  
                break; 
            }
        }
    }

    
    for(int i=0;i<s.size();i++){
        risBug[i]=s[i];
    }
    risBug[s.size()]=0;
}

    

double boardEval(EBoard* b){

    // TODO: inizia da qui per capire una buona euristica. Vedi chat

    /*
    STARTING TIPS:

    WHITE:

    Turn 1: Place wG1

    Turn 3: Place wQ to form elbow:
    - If opponent is on your right → place Queen on left of wG1
    - If opponent is on your left → place Queen on right of wG1

    Turn 5: Place wA1 toward where you expect enemy Queen

    BLACK:

    Turn 2: Place bS1 adjacent to wG1 (only one legal choice)

    Turn 4: Place bQ next to bS1 (form inline or elbow shape)
        - Prefer inline (bQ opposite wG1 from bS1), unless blocked

    Turn 6: Add bA1 near bQ
    
    */

    //return 0;
    
    PlayerColor MyCol=b->board_exp.currentColor();
    pieceT myQueen=(MyCol==PlayerColor::WHITE)?8:22;
    pieceT oppositeQueen=30-myQueen;
    
    int nearMyQueen=0;
    int nearOpposite=0;
    positionT mqP=b->board_exp.G.getPosition(myQueen);
    positionT oqP=b->board_exp.G.getPosition(oppositeQueen);
    if(!b->board_exp.G.isPlaced[myQueen]){
        nearMyQueen=5;
    }else{
        for(direction i=0;i<6;i++)
            if(!b->board_exp.G.isFree(applayMove(mqP,i))){
                nearMyQueen++;
            }
    }
    if(!b->board_exp.G.isPlaced[oppositeQueen]){
        nearOpposite=5;
    }else{
        for(direction i=0;i<6;i++)
            if(!b->board_exp.G.isFree(applayMove(oqP,i))){
                nearOpposite++;
            }
    }
    
    
    int movableSelf=0, movableOpp=0;
    for(int p=1;p<=28;p++){
        if(b->board_exp.G.isPlaced[p] && b->board_exp.G.canMoveWithoutBreakingHiveRule(p,b->board_exp.currentTurn+1))
        {
            if(col(p)==MyCol)movableSelf++;
            else movableOpp++;
        }
    }

    double score = 0.0;
    

    score += 1 * movableOpp ;
    score += -2 * nearMyQueen;
    score += 2 * nearOpposite;
    
    return score;
}


void delBoard(EBoard* b){
    delete b;
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
