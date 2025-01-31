
#include <math.h>
#include "board.h"
#include <iostream>

// Define export macro for Windows
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

Board b;

void raise_exception() {
    b.reset();
}

EXPORT int startGame(char* s) {
    //Type of string:

    // it can have GameTypeString (Base+MLP) or Gamestring (Gametype;State;Player[Turn];moves...)

    //Base;InProgress;White[3];wS1;bG1 -wS1;wA1 wS1/;bG2 /bG1
    //Base+MLP

    //GameTypeString
    if (s == nullptr) {
        raise_exception();
        return 1;
    }

    if (strncmp(s,"Base",4) != 0) {
        raise_exception();
        return 1;
    }
    s += 4;

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
            b.type = Base_M;
        }
        if (*s == 'L') {
            b.addPieceHand(piece{LADYBUG,BLACK,0});
            b.addPieceHand(piece{LADYBUG,WHITE,0});
            s++;
            if (b.type == Base_M) {
                b.type = Base_ML;
            } else {
                b.type = Base_L;
            }
        }
        if (*s == 'P') {
            b.addPieceHand(piece{PILLBUG,BLACK,0});
            b.addPieceHand(piece{PILLBUG,WHITE,0});
            s++;
            if (b.type == Base_M) {
                b.type = Base_MP;
            } else if (b.type == Base_L) {
                b.type = Base_LP;
            } else if (b.type == Base_ML) {
                b.type = Base_MLP;
            } else {
                b.type = Base_P;
            }
        } 
    }

    b.state = STARTED;



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
            return 1;
        }
        *s = 0;
        s++;

        //TurnString
        char* turn = s;
        while (*s != '[' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            raise_exception();
            return 1;
        }
        *s = 0;
        int curTurn = 0;
        if (!strcmp(turn,"White")) {
            curTurn = 1;
        } else if (!strcmp(turn,"Black")) {
            curTurn = 2;
        } else {
            raise_exception();
            return 1;
        }
        s++;
        turn = s;
        while (*s != ']' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            raise_exception();
            return 1;
        }
        *s = 0;
        curTurn = curTurn + 2*(atoi(turn)-1);
        if (curTurn <= 0) {
            raise_exception();
            return 1;
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
            b.executeAction(move);
            if (ret) {
                break;
            }
            *s = ';';
        }

        //Check if the current status is true
        if (b.currentTurn != curTurn || b.state != parseState(state)) {
            raise_exception();
            return 1;
        }
    }
    return 0;
}

EXPORT int playMove(char* m) {
    if(b.executeAction(m)) {
        cout << "-------------------" << endl;
        if (b.state == DRAW) {
            return 1;
        } else if (b.state == WHITE_WIN){
            return 2;
        } else if (b.state == BLACK_WIN){
            return 3;
        }
    }
    cout << "-------------------" << endl;
    return 0;
}

static char BUFFER[100000];  // Buffer used both for validMoves and getBoard

EXPORT char* validMoves() {
    try {
        vector<action> moves = b.possibleMoves();

        if (b.state == NOT_STARTED) {
            strcpy(BUFFER, "err: The game has not started yet");
            return BUFFER;
        } else if (b.state != IN_PROGRESS && b.state != STARTED) {
            strcpy(BUFFER, "err: The game is over");
            return BUFFER;
        }

        if (moves.size() == 0) {
            strcpy(BUFFER, "pass");
            return BUFFER;
        }
        
        int i = 0;
        for(const action& move : moves) {
            string item = ActionToString(move);
            int len = item.size();
            // Check buffer bounds
            if ( (long long unsigned int) i + len + 1 >= sizeof(BUFFER)) {
                // Buffer overflow protection
                BUFFER[0] = '\0';
                return BUFFER;
            }
            memcpy(BUFFER + i, item.c_str(), len);
            i += len;
            BUFFER[i] = ';';
            i++;
        }

        // Replace last semicolon with null terminator
        BUFFER[i-1] = '\0';
        return BUFFER;
    }
    catch (const std::exception&) {
        BUFFER[0] = '\0';
        return BUFFER;
    }
}

EXPORT const char* getBoard() {    
    static char BUFFER[10000];
    
    try {
        // First test with a simple static string
        string test = b.toString();
        strcpy(BUFFER, test.c_str());
        return BUFFER;
    }
    catch (const std::exception&) {
        BUFFER[0] = '\0';
        return BUFFER;
    }
}

EXPORT void undo(int amount) {
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

*/