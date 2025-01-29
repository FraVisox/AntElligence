#include "board.cpp"

Board b;
/**
 * Sets up the game based on the given string.
 *
 * This function is responsible for setting up the board based on the
 * given string. It will read the string and interpret it as a game
 * state. The game state will be passed to the constructor of the Board
 * class, which will then set up the board correctly.
 *
 * @param s A string representing the game state.
 */
void startGame(char* s){  // read the string and setup the game
    if (strlen(s) > 4) {
        s = s+4;
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

    // If there wasn't any extension, remove 'Base'.
    if (*s == 'B') { 
        s = s+4;
    }

    // The remaining string is of the type: ;InProgress;White[3];wS1;bG1 -wS1;wA1 wS1/;bG2 /bG1
    // TODO: check if it is the right format
    if (*s == ';') {
        //Read state
        s = s+1;
        char* state = s;
        while (*s != ';') {
            s++;
        }
        s = 0;
        s++;

        //Read turn
        char* turn = s;
        while (*s != '[') {
            s++;
        }
        s = 0;
        int curTurn = 0;
        if (!strcmp(turn,"White")) {
            curTurn = 1;
        } else if (!strcmp(turn,"Black")) {
            curTurn = 2;
        }
        s++;
        turn = s;
        while (*s != ']') {
            s++;
        }
        s = 0;
        curTurn = curTurn + 2*(atoi(turn)-1);

        s++;

        //Read moves and execute
        while (s == 0) {
            s++;
            char* move = s;
            while (*s != ';' && *s != 0) {
                s++;
            }
            s = 0;
            b.executeAction(StringToAction(move));
        }


        //Check if the current status is true
        if (b.currentTurn != curTurn || b.state != StringToState(state)) {
            // TODO: what to do if the current status is not true. Raise an exception
            return;
        }

    }
}


void playMove(char* m){   //execute this move
    b.executeAction(StringToAction(m));
}

char* allMoves(){  // return all the possible moves in the current configuration
    vector<action> moves = b.possibleMoves();
    
    char *h = new char[moves.size()*9+5];
    int i = 0;
    for(action move : moves){
        char* item = MovementToString(move);
        int len = strlen(item);
        for (int j = 0; j < len; j++) {
            h[i+j] = item[j];
        }
        i += len;
        h[i] = ';';
        i++;
    }
    if (i == 0) {
        h = "pass;";
    }
    return h;
}


char* getBoard() {
    return b.toString();
}

void undo(int amount) {
    b.undo(amount);
}