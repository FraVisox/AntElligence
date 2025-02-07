
#include <math.h>
#include "board.h"
#include <iostream>
#include <regex>

/*

//TODO: be more precise with error types

//TODO: OTHER RULES

The game ends in a draw in the following cases: 
a) both Queen Bees are surrounded at the same at the end of any turn; 
b) it is mutually agreed upon; 
c) the same position appears three times over the whole game. (***********************************)

The pieces already in play can be moved according to their abilities, provided the following are all true:
- The player’s Queen Bee is in the hive;
- The activated piece and the piece being moved are both uncovered;
- Neither the activated piece, nor the piece being moved, were moved on the (********************************)
previous turn;
- Removing the piece being moved from the hive would not cause the hive to become discontiguous (One Hive rule).

*/

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

/**
 * Starts a new game or resumes a game from a given GameString.
 *
 * This function initializes a new board based on the provided GameString, which 
 * includes game type, game state, turn information, and any moves made so far.
 * It validates the GameString format and updates the game board accordingly.
 * If the GameString is invalid or inconsistent, the function returns an error.
 *
 * @param s A character pointer representing the GameString.
 *          The GameString should follow a specific pattern to indicate 
 *          the game configuration and moves.
 * @return An integer indicating success or error:
 *         - Returns 0 if the game is successfully started or resumed.
 *         - Returns ERROR if the GameString is invalid or inconsistent.
 */
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

  EXPORT int getTurn(){
    return b.currentTurn;
}

EXPORT double oracleEval(){
    return 0; // TODO
}

/**
 * \brief Execute a move in the game.
 *
 * Given a string representing a move, this function executes it in the current game.
 * The string should be a valid MoveString, which is a concatenation of a bug piece
 * to be moved and its relative position and direction. The relative position and
 * direction are represented by a single string, which is the concatenation of the
 * relative position bug piece and the direction indicator.
 *
 * \param m The string representing the move.
 * \return 0 if the move was successful, an error code otherwise.
 */
EXPORT int playMove(char* m) {
    return b.executeAction(m);
}

// IT SHOULD BE SUFFICIENT FOR ALL THE POSSIBLE MOVES (considering 9 bytes for each move)
static char MOVES_BUFFER[10000];

/**
 * \brief Return a string with all valid moves for the current player.
 *
 * If the game has not started yet, this function returns "err The game has not started yet.".
 * If the game is over, this function returns "err The game is over.".
 * If there are no valid moves, this function returns "pass".
 *
 * Otherwise, this function returns a string with all valid moves for the current player.
 * The string is a concatenation of valid moves, separated by semicolons.
 * Each valid move is represented by a MoveString, which is a concatenation of a bug piece
 * to be moved and its relative position and direction. The relative position and
 * direction are represented by a single string, which is the concatenation of the
 * relative position bug piece and the direction indicator.
 *
 * \return A pointer to a string containing all valid moves for the current player.
 */
EXPORT char* validMoves() {
    vector<action> moves = b.possibleMoves();

    if (b.state == NOT_STARTED) {
        strcpy(MOVES_BUFFER, "err The game has not started yet.");
        return MOVES_BUFFER;
    } else if (b.state != IN_PROGRESS && b.state != STARTED) {
        strcpy(MOVES_BUFFER, "err The game is over.");
        return MOVES_BUFFER;
    }

    if (moves.size() == 0) {
        strcpy(MOVES_BUFFER, "pass");
        return MOVES_BUFFER;
    }
    
    int i = 0;
    for(const action& move : moves) {
        string item = ActionToString(move);
        int len = item.size();
        // Check buffer bounds
        if ( (long long unsigned int) i + len + 1 >= sizeof(MOVES_BUFFER)) {
            // Buffer overflow protection
            MOVES_BUFFER[0] = '\0';
            return MOVES_BUFFER;
        }
        memcpy(MOVES_BUFFER + i, item.c_str(), len);
        i += len;
        MOVES_BUFFER[i] = ';';
        i++;
    }

    // Replace last semicolon with null terminator
    MOVES_BUFFER[i-1] = '\0';
    return MOVES_BUFFER;
}

//This should take into account around 10 000 turns
static char BOARD_BUFFER[100000];

/**
 * \brief Returns a string representation of the current game state.
 *
 * This string contains all information about the current game state, including
 * the current turn number, the game type, the game state, the moves made so far,
 * the bugs currently in play and the bugs in hand.
 *
 * The string is a semicolon-separated list of the following elements:
 * - The game type (Base, Base_M, Base_L, Base_P, Base_ML, Base_MP, Base_LP, Base_MLP)
 * - The game state (NOT_STARTED, STARTED, IN_PROGRESS, DRAW, WHITE_WIN, BLACK_WIN)
 * - The current turn number
 * - A list of the moves made so far, each move being a MoveString
 * - The bugs currently in play, each bug being represented by a BugString
 * - The bugs in hand, each bug being represented by a BugString
 *
 * \return A pointer to a string containing the current game state.
 */
EXPORT const char* getBoard() {        
    string test = b.toString();
    strcpy(BOARD_BUFFER, test.c_str());
    return BOARD_BUFFER;
}

/**
 * \brief Undoes the specified amount of moves on the board.
 *
 * This function calls the undo method on the board to reverse the 
 * specified number of moves. It updates the game state accordingly.
 *
 * \param amount The number of moves to undo.
 */
EXPORT void undo(int amount) {
    b.undo(amount);
}





#ifdef __cplusplus
}
#endif

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