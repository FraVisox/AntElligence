#include "engine.h"
#include "engine/enums.h"
#include "engine/board.h"
#include <string>
using namespace std;

string actionToString(actionT a, Board &board){
    if(a==0){
        return "pass";
    }
    
    pieceT bug=a&31;
    positionT pos=a>>5;
    string s=PiecetoString(bug);
    //cout<<" read to "<<bug<<" in pos "<<pos<<" -> ";
    gameboard& g=board.G;
    if(!g.isFree(pos)){
        cout<<"FP";
        pieceT resPiece=board.G.topPiece(pos);   
        s+=" "+nameDirToString(PiecetoString(resPiece),opposite(6));  
        //cout<<" OK  with place over"<<endl;
        return s;
    }else{
        //cout<<" LN ";
        for(int dir=0;dir<6;dir++){
            positionT next=applayMove(pos,dir);
            //cout<<" ( "<<next<<"->";
            int h=g.getHight(next);
            if(h>0){
                //cout<<" O ";
                if(g.topPiece(next)==bug){
                    //cout<<"ME ";
                    if(h==1){
                       // cout<<"X)";
                    }else{
                        pieceT resPiece=g.gb[h-2][next];
                        s+=" "+nameDirToString(PiecetoString(resPiece),opposite(dir));  
                     //   cout<<"go down)"<<endl;
                        return s;
                    }
                }else{
                    pieceT resPiece=g.topPiece(next);
                    s+=" "+nameDirToString(PiecetoString(resPiece),opposite(dir));  
                  //  cout<<"Ok)"<<endl;
                    return s; 
                }

            }
         //   cout<<"X) ,";
        }
    }
 //   cout<<" No -> place free";
    return s;
//    return s;
}

actionT stringToAction(Board &b, string str){
    pieceT startP,destP;
    direction dir ;

    if(str[0]=='p')return pass();

    startP=decodeBug((char*)str.c_str());

    int q=1; 
    while(str[q]!='w' && str[q]!='b' && str[q]!=0) q++;

    if(str[q]==0) return placeFirst(startP);
    
    destP=decodeBug((char*)str.c_str()+q);

    q=2;
    if(str[q]=='1' || str[q]=='2' || str[q]=='3') q++;

    while(str[q]==' ') q++;

    if(str[q]=='w' || str[q]=='b'){  // is the case BUG BUG dir;
        q+=2; if(str[q]>'0' && str[q]<'4')q++;
        switch(str[q]){
            case '/': dir=5;break;
            case '-': dir=0;break;
            case '\\': dir=1;break;
            case 0: dir=6;break;
            default : throw "WTF??";
        }
    }else{
        if(str[q]>'0' && str[q]<'4')q++;
        switch(str[q]){
            case '/': dir=2;break;
            case '-': dir=3;break;
            case '\\': dir=4;break;
            default: throw "NOT A VALID STRING";
        }
    }
    //cout<<"Conver"<< str<<" to :"<<0+startP<<","<<0+destP<<" "<<dir<<endl;
    positionT destPos=applayMove(b.G.getPosition(destP),dir);
    return movement(startP,destPos);
}

void Engine::newgame(string s){
    board = Board(Base_MLP);
    actions.clear();
    printBoard();
}
   

void Engine::validmoves(){
    if (board.getGameState() == NOT_STARTED)
        error("Game has not started yet");
    else if (board.getGameState() != STARTED)
        error("The game is over");

    board.ComputePossibleMoves();

    if(board.numAction==0){
        cout<<"pass\n";
    }

    for(int i=0;i<board.numAction-1;i++){
        cout<<actionToString(board.resAction[i], board)<<";";
    }
    cout<<actionToString(board.resAction[board.numAction-1], board)<<"\n";
}

void Engine::bestmove(string param){
    if (board.currentTurn == 1) {
        cout << "wG1\n";
    } else if (board.currentTurn == 2) {
        cout << "bS1 -"+actions[0]+"\n";
    } else {
        cout << actionToString(agent.calculate_best_move(board), board)<<"\n";
    }
}

void Engine::play(string move){
    board.applayAction(stringToAction(board, move));
    actions.push_back(move);
    printBoard();
}

void Engine::printBoard(){
    cout<<board.toString();
    for (string el : actions) {
        cout<< ";" << el;
    }
    cout<<"\n";
}