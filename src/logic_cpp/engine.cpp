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

        /* TODO:
            add the remaining pieces based on the string.
        */
        b.addPieceHand(piece{MOSQUITO,col,0});
        b.addPieceHand(piece{LADYBUG,col,0});
        b.addPieceHand(piece{PILLBUG,col,0});
    }    

    /* TODO:
        read the string and play the moves already played.
    */
}


void playMove(char* m){   //execute this move

}

char* allMoves(){  // return all the possible moves in the current configuration
    char c[]="avnifjufrsbin";
    char *h=new char[20];
    for(int i=0;i<4;i++){
        h[i]=c[i];
    }
    b.possibleMoves();
    return h;
}



int main(){
    startGame(0);
    cout<<"ok";
    
    return 0;
}