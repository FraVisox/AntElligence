#include "board.cpp"



Board b;
void startGame(char* s){  // read the string and setup the game
    for(PlayerColor col : {BLACK,WHITE}){
        b.addPieceHand(piece{QUEEN,col,0});
        for(int i=1;i<=3;i++){
            b.addPieceHand(piece{SPIDER,col,i});
            b.addPieceHand(piece{BEETLE,col,i});
            b.addPieceHand(piece{GRASSHOPPER,col,i});
            b.addPieceHand(piece{SOLDIER_ANT,col,i});
        }
    }

    // TODO : Adapt setup for variants
    
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