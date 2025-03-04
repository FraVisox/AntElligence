#include "engine.h"
#include <string>
using namespace std;

void Engine::newgame(const char* s){
    //TODO: check for correctness

    //Create newboard as to not modify the original
    Board newb = Board();
    newb.type = Base;

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

    if (s == nullptr) { //means it has no string
        board.copy(newb);
        return;
    }

    //GameTypeString
    s += 4; //Skip "Base"

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
        const char* state = s;
        while (*s != ';' && *s != 0) {
            s++;
        }
        //The delimiter will be with ;
        s++;

        //TurnString
        const char* turn = s;
        while (*s != '[' && *s != 0) {
            s++;
        }
        //The delimiter will be [

        int curTurn = 0;
        if (!strncmp(turn,"White", 5)) {
            curTurn = 1;
        } else if (!strncmp(turn,"Black", 5)) {
            curTurn = 2;
        }
        s++;
        turn = s;
        int tocopy = 0;
        while (*s != ']' && *s != 0) {
            s++;
            tocopy++;
        }

        //The delimiter will be ]
        char substr[8];
        memcpy(substr, turn, tocopy);
        substr[tocopy+1] = '\0';

        curTurn = curTurn + 2*(atoi(substr)-1);
        s++;

        //MoveString
        while (*s == ';') {
            s++;
            const char* move = s;
            int tocopy = 0;
            while (*s != ';' && *s != 0) {
                s++;
                tocopy++;
            }
            bool ret = false;
            if (*s == 0) {
                ret = true;
            }

            char this_move[9];
            memcpy(this_move, move, tocopy);
            this_move[tocopy+1] = 0;
            newb.executeAction(this_move);
            if (ret) {
                break;
            }
        }
    }

    board.copy(newb);

    cout << board.toString() << "\n";
}
   

void Engine::validmoves(){
    if (board.state == NOT_STARTED)
        error("Game has not started yet");
    else if (board.state != IN_PROGRESS && board.state != STARTED)
        error("The game is over");

    vector<action> moves=board.possibleMoves();
    if(moves.size()==0){
        cout<<"pass\n";
    }

    for(int i=0;i<moves.size()-1;i++){
        cout<<ActionToString(moves[i])<<";";
    }
    cout<<ActionToString(moves[moves.size()-1])<<"\n";
}

void Engine::bestmove(string param){
    cout << ActionToString(agent.calculate_best_move(board))<<"\n";
}

void Engine::play(string move){
    board.executeAction(move);
    cout<<board.toString()<<"\n";
}

void Engine::undo(string arguments="1"){
    board.undo(stoi(arguments));
    cout<<board.toString()<<"\n";
}