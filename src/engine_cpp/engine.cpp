
#include <math.h>
#include "board.h"
#include <iostream>
#include <regex>

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

/*
    TODO: BUGS

*/


//TODO: be more precise with error types

EXPORT int startGame(char* s) {
    //Check the string:
    if (s == nullptr) {
        return ERROR;
    }
    string ss = s;
    const std::regex game_string_pattern(R"(^Base(?:\+(?:M|L|P|ML|LP|MP|MLP))?(?:;(?:InProgress|NotStarted|Draw|WhiteWins|BlackWins)(?:;(?:White|Black)\[\d+\](?:;(?:(?:w|b)[QSBGAMLP](?:[1-3])?(?: [\\\-/](?:w|b)[QSBGAMLP](?:[1-3])?)?(?: (?:w|b)[QSBGAMLP](?:[1-3])?[\\\-/])?)?)*)?)?$)");
    if (!std::regex_match(ss, game_string_pattern)) {
        return ERROR;
    }

    //Create newboard as to not modify the original
    Board newb = Board();
    newb.type = Base;

    //GameTypeString
    s += 4; //Skip "Base"

    //Add pieces
    for(PlayerColor col : {BLACK,WHITE}){
        newb.addPieceHand(piece{QUEEN,col,0});
        for(int i=1;i<=2;i++){
            newb.addPieceHand(piece{SPIDER,col,i});
            newb.addPieceHand(piece{BEETLE,col,i});
            newb.addPieceHand(piece{GRASSHOPPER,col,i});
            newb.addPieceHand(piece{SOLDIER_ANT,col,i});
        }
        newb.addPieceHand(piece{GRASSHOPPER,col,3});
        newb.addPieceHand(piece{SOLDIER_ANT,col,3});
    } 

    //If there are other pieces
    if (*s == '+') {
        s++;  
        if (*s == 'M') {
            newb.addPieceHand(piece{MOSQUITO,BLACK,0});
            newb.addPieceHand(piece{MOSQUITO,WHITE,0});
            s++;
            newb.type = Base_M;
        }
        if (*s == 'L') {
            newb.addPieceHand(piece{LADYBUG,BLACK,0});
            newb.addPieceHand(piece{LADYBUG,WHITE,0});
            s++;
            if (newb.type == Base_M) {
                newb.type = Base_ML;
            } else {
                newb.type = Base_L;
            }
        }
        if (*s == 'P') {
            newb.addPieceHand(piece{PILLBUG,BLACK,0});
            newb.addPieceHand(piece{PILLBUG,WHITE,0});
            s++;
            if (newb.type == Base_M) {
                newb.type = Base_MP;
            } else if (newb.type == Base_L) {
                newb.type = Base_LP;
            } else if (newb.type == Base_ML) {
                newb.type = Base_MLP;
            } else {
                newb.type = Base_P;
            }
        } 
    }

    newb.state = STARTED;

    //If there is also a game in progress
    if (*s == ';') {
        s++;


        //GameStateString
        char* state = s;
        while (*s != ';' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            return ERROR;
        }
        *s = 0;
        s++;


        //TurnString
        char* turn = s;
        while (*s != '[' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            return ERROR;
        }
        *s = 0;
        int curTurn = 0;
        if (!strcmp(turn,"White")) {
            curTurn = 1;
        } else if (!strcmp(turn,"Black")) {
            curTurn = 2;
        } else {
            return ERROR;
        }
        s++;
        turn = s;
        while (*s != ']' && *s != 0) {
            s++;
        }
        if (*s == 0) {
            return ERROR;
        }
        *s = 0;
        curTurn = curTurn + 2*(atoi(turn)-1);
        if (curTurn <= 0) {
            return ERROR;
        }
        s++;

        //MoveString
        while (*s == ';') {
            s++;
            char* move = s;
            while (*s != ';' && *s != 0) {
                s++;
            }
            bool ret = false;
            if (*s == 0) {
                ret = true;
            }
            *s = 0;
            if (newb.executeAction(move) == ERROR) {
                return ERROR;
            } 
            if (ret) {
                break;
            }
            *s = ';';
        }

        //Check if the current status is true
        if (newb.currentTurn != curTurn || (parseState(state) != NOT_STARTED && newb.state != parseState(state)) || (parseState(state) == NOT_STARTED && newb.state != STARTED)) {
            return ERROR;
        }
    }

    b.copy(newb);
    return 0;
}

EXPORT int playMove(char* m) {
    return b.executeAction(m);
}

// TODO: is it sufficient? 14 bugs, even with 10 moves a bug we should have at max 1260 bytes. Maybe find a better lower bound
static char BUFFER[10000];  // Buffer used both for validMoves and getBoard. 

EXPORT char* validMoves() {
    vector<action> moves = b.possibleMoves();

    if (b.state == NOT_STARTED) {
        strcpy(BUFFER, "err The game has not started yet.");
        return BUFFER;
    } else if (b.state != IN_PROGRESS && b.state != STARTED) {
        strcpy(BUFFER, "err The game is over.");
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

EXPORT const char* getBoard() {        
    string test = b.toString();
    strcpy(BUFFER, test.c_str());
    return BUFFER;
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