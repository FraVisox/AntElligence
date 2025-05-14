#ifndef INTERFACE_H
#define INTERFACE_H


#include "embadded_board.h"


// Forward declaration of the board structure
struct EBoard;

// Creates a new board for the given game type
EBoard* base_state(int game_type);

// Copies an existing board
EBoard* copyBoard(EBoard* board);

// Frees the memory for a board
void delBoard(EBoard* board);

// Applies an action to the board
void next_state(EBoard* board, actionT action);

// Fills the given array with legal actions for the board
void getActions(EBoard* board, actionT* actions);

// Returns the status of the game
int checkStatus(EBoard* board);

// Converts an action to a string for the given board
void actionToString(actionT action, EBoard* board,char* v);

// Converts a string to an action
actionT stringToAction(EBoard* board,  char* str);

// Prints a human-readable version of an action

// Prints the board to stdout
void PrintBoard(EBoard* board);

// Evaluates the board and writes the result into the pointer
double boardEval(EBoard* board, double* w);

// Returns a raw pointer to the board vector (284 bytes)
 char* BoardRapp(EBoard* board);

// Optional: Create a board from a raw NumPy-compatible array
EBoard* fromArray( char* flat_array);  // You must implement this in interface.cpp


#endif  // INTERFACE_H
