
#include <math.h>
#include "board.h"

// Define export macro for Windows
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Export all functions explicitly
EXPORT double _sin(double x) {
    return sin(x);
}


Board b;

void raise_exception() {
    b.reset();
    throw std::exception();
}

EXPORT void startGame(char* s) {
    // Your existing startGame implementation
    
    //Type of string:

    // it can have GameTypeString (Base+MLP) or Gamestring (Gametype;State;Player[Turn];moves...)

    //Base;InProgress;White[3];wS1;bG1 -wS1;wA1 wS1/;bG2 /bG1
    //Base+MLP

    //GameTypeString

    if (strncmp(s,"Base",4)) {
        raise_exception();
    }

    for(PlayerColor col : {BLACK,WHITE}){
        b.addPieceHand(piece{QUEEN,col,0});
        for(int i=1;i<=2;i++){
            b.addPieceHand(piece{SPIDER,col,i});
            b.addPieceHand(piece{BEETLE,col,i});
            b.addPieceHand(piece{GRASSHOPPER,col,i});
            b.addPieceHand(piece{SOLDIER_ANT,col,i});
        }
        b.addPieceHand(piece{GRASSHOPPER,col,3});
        b.addPieceHand(piece{SOLDIER_ANT,col,3});
    } 

    if (*s == '+') {
        s++;  
        if (*s == 'M') {
            b.addPieceHand(piece{MOSQUITO,BLACK,0});
            b.addPieceHand(piece{MOSQUITO,WHITE,0});
            s++;
        }
        if (*s == 'L') {
            b.addPieceHand(piece{LADYBUG,BLACK,0});
            b.addPieceHand(piece{LADYBUG,WHITE,0});
            s++;
        }
        if (*s == 'P') {
            b.addPieceHand(piece{PILLBUG,BLACK,0});
            b.addPieceHand(piece{PILLBUG,WHITE,0});
            s++;
        } 
    }


    //GameString
    if (*s == ';') {
        s++;

        //GameStateString
        char* state = s;
        while (*s != ';' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            raise_exception();
        }
        s = 0;
        s++;

        //TurnString
        char* turn = s;
        while (*s != '[' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            raise_exception();
        }
        s = 0;
        int curTurn = 0;
        if (!strcmp(turn,"White")) {
            curTurn = 1;
        } else if (!strcmp(turn,"Black")) {
            curTurn = 2;
        } else {
            raise_exception();
        }
        s++;
        turn = s;
        while (*s != ']' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            raise_exception();
        }
        s = 0;
        curTurn = curTurn + 2*(atoi(turn)-1);
        if (curTurn <= 0) {
            raise_exception();
        }
        s++;

        //MoveString
        while (*s == ';') {
            char* move = s+1;
            while (*s != ';' && *s != 0) {
                s++;
            }
            bool ret = false;
            if (*s == 0) {
                ret = true;
            }
            *s = 0;
            b.executeAction(parseAction(move));
            if (ret) {
                break;
            }
            *s = ';';
        }

        //Check if the current status is true
        if (b.currentTurn != curTurn || b.state != parseState(state)) {
            raise_exception();
        }
    }
}

EXPORT int playMove(char* m) {
    if(b.executeAction(parseAction(m))) {
        if (b.state == DRAW) {
            return 1;
        } else if (b.state == WHITE_WIN){
            return 2;
        } else if (b.state == BLACK_WIN){
            return 3;
        }
    }
    return 0;
    // Your existing playMove implementation
}

EXPORT char* validMoves() {
    vector<action> moves = b.possibleMoves();
    
    char *h = new char[moves.size()*9+5];
    int i = 0;
    for(action move : moves){
        string item = MovementToString(move);
        int len = item.size();
        for (int j = 0; j < len; j++) {
            h[i+j] = item[j];
        }
        i += len;
        h[i] = ';';
        i++;
    }

    if (i == 0) { //Only if it has no other move
        h[0] = 'p';
        h[1] = 'a';
        h[2] = 's';
        h[3] = 's';
        h[4] = 0;
    } else { //Remove the last semicolon
        h[i-1] = 0;
    }
    return h;
    // Your existing validMoves implementation
}

EXPORT char* getBoard() {
    // Your existing getBoard implementation
    return b.toString();
}

EXPORT void undo(int amount) {
    // Your existing undo implementation
    b.undo(amount);
}

#ifdef __cplusplus
}
#endif

/* STRINGS

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

*



void raise_exception() {
    b.reset();
    throw std::exception();
}

/**
 * Executes a move on the board using the given MoveString.
 *
 * This function interprets the MoveString, converts it into an action,
 * and executes the action on the current board state.
 *
 * @param m A MoveString representing the move to be played.
 *
int playMove(char* m){
    if(b.executeAction(parseAction(m))) {
        if (b.state == DRAW) {
            return 1;
        } else if (b.state == WHITE_WIN){
            return 2;
        } else if (b.state == BLACK_WIN){
            return 3;
        }
    }
    return 0;
}

/**
 * Returns a string containing all possible moves for the current player.

 * The moves are represented as a series of MoveStrings, separated by semicolons.

 * @return A string containing all possible moves for the current player.
 *
char* validMoves(){

    vector<action> moves = b.possibleMoves();
    
    char *h = new char[moves.size()*9+5];
    int i = 0;
    for(action move : moves){
        string item = MovementToString(move);
        int len = item.size();
        for (int j = 0; j < len; j++) {
            h[i+j] = item[j];
        }
        i += len;
        h[i] = ';';
        i++;
    }

    if (i == 0) { //Only if it has no other move
        h[0] = 'p';
        h[1] = 'a';
        h[2] = 's';
        h[3] = 's';
        h[4] = 0;
    } else { //Remove the last semicolon
        h[i-1] = 0;
    }
    return h;
}



/**
 * Returns a string containing the current state of the board.
 *
 * The returned string is a GameString, which is a semicolon-separated
 * list of the board's properties.
 *
 * @return A string containing the current state of the board.
 *
char* getBoard() {    
    return b.toString();
}

/**
 * Undoes the specified amount of moves.
 *
 * @param amount The number of moves to undo.
 *
void undo(int amount) {
    b.undo(amount);
}

*/