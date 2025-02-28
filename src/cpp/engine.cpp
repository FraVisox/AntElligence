#include "engine.h"
#include <string>
using namespace std;

void Engine::newgame(string arguments){
    vector<string> gameInfos;    
    if(arguments.empty())arguments="Base";

    int p=0;
    while(p < arguments.size()){
        int n=arguments.find(';',p);
        if(n==-1){
            gameInfos.push_back(arguments.substr(p));
            break;
        }
        gameInfos.push_back(arguments.substr(p,n-p));
        p=n+1;
    }
    board.state=STARTED;
    board.reset();

    for(PlayerColor col : {BLACK,WHITE}){
        board.addPieceHand(piece{QUEEN,col,0});
        for(int i=1;i<=2;i++){
            board.addPieceHand(piece{SPIDER,col,i});
            board.addPieceHand(piece{BEETLE,col,i});
            board.addPieceHand(piece{GRASSHOPPER,col,i});
            board.addPieceHand(piece{SOLDIER_ANT,col,i});
        }
        board.addPieceHand(piece{GRASSHOPPER,col,3});
        board.addPieceHand(piece{SOLDIER_ANT,col,3});
    }
    
    board.type=Base;
    if(gameInfos[0].size()>4){
        string addInfo=gameInfos[0].substr(5);  

        if (addInfo.find('M')!=-1) {
            board.addPieceHand(piece{MOSQUITO,BLACK,0});
            board.addPieceHand(piece{MOSQUITO,WHITE,0});
            board.type = Base_M;
        }
        if (addInfo.find('L')!=-1) {
            board.addPieceHand(piece{LADYBUG,BLACK,0});
            board.addPieceHand(piece{LADYBUG,WHITE,0});
            if (board.type == Base_M) {
                board.type = Base_ML;
            } else {
                board.type = Base_L;
            }
        }
        if (addInfo.find('P')!=-1) {
            board.addPieceHand(piece{PILLBUG,BLACK,0});
            board.addPieceHand(piece{PILLBUG,WHITE,0});
            if (board.type == Base_M) {
                board.type = Base_MP;
            } else if (board.type == Base_L) {
                board.type = Base_LP;
            } else if (board.type == Base_ML) {
                board.type = Base_MLP;
            } else {
                board.type = Base_P;
            }
        }
    }

    if(gameInfos.size()==1 || gameInfos[1]=="NotStarted"){

        cout<<board.toString()<<"\n";
        return;
    }
    //if(gameInfos[1]=="InProgress"){
    //        board.currentTurn=2*stoi(gameInfos[2].substr(6,gameInfos[2].size()-7));
    //    if(gameInfos[2][0]=='W'){
    //        board.currentTurn-=1;
    //    }
    // }

    for(int j=3;j<gameInfos.size();j++){
        board.executeAction(gameInfos[j]);
    }
    cout<<board.toString()<<"\n";
    board.state=STARTED;
}
   

void Engine::validmoves(){

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

void Engine::undo(string arguments="0"){
    board.undo(stoi(arguments));
    cout<<board.toString()<<"\n";
}