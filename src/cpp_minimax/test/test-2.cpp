#include "../engine/board.h"
#include "../engine.h"
#include <chrono>
#include <iostream>


using namespace std;

void printBoard(Board& board);

int main(){
    
        
    auto seed=time(0);

    for(int TR=0;TR<100;TR++){
        cout<<"Run with seed:"<<seed+TR<<endl;
        srand(seed+TR);

        Board eb(GameType::Base_MLP);
        int rt;
        int i=0;
        try{
            while(true){
                eb.ComputePossibleMoves();
                
                int m=rand()%eb.numAction;
                //m=1;
                actionT nextAct=eb.resAction[m];
                string s=actionToString(nextAct,eb);
                actionT nAct=stringToAction(eb,s);
                    
                //    clog<<endl<<"Action "<<i;
                if(nAct!=nextAct ){
                    cout<<"Error in conversion\n";
                    clog<<"Valid moves: ";
                    clog<<endl;
                    cout<<"Action "<<i<<endl;
                    cout<<"Error in conversion\n";
                    cout<<nextAct<<" -> "<<s<<" -> "<<nAct<<endl;
                    clog<<nextAct<<" -> "<<s<<" -> "<<nAct<<endl;
                }

                //cout<<"\nAction "<<i<< ": "<<s<<endl;

                //printBoard(eb);
                //eb.G.occupied.printDBG();
                eb.applayAction(nextAct);
                i++;
                cout<<"\n";
                
                //printBoard(eb);


                //eb.checkConsistency();
                if(eb.getGameState()>1){
                    clog<<"Tot act"<<i<<endl;
                    break;
                }

            }
        }catch (exception e){
            cout<<"ERRORE :"<<e.what()<<endl;
            cout<<"More info : i="<<i<<", rt="<<rt<<", seed="<<seed<<endl;
        }
    }
    cout<<"All pass"<<endl;
}



void printBoard(Board& board){
    return;
    cout<<"BOARD:"<<endl;
    for(int i=16;i<48;i++){
        printf("%4d",(32*i+16)&1023);
        if(i<10)cout<<" ";
        for(int j=16;j<i;j++)std::cout<<"  ";
        for(int j=16;j<48;j++){
            gameboard &g=board.G;
            if(g.high[(j+32*i)&1023]==0){
                //printf("%3d", ((j+32*i)&1023)%1000);
                cout<<"   ";
            }
            if(g.high[(j+32*i)&1023]>1){
                cout<<"XXX";
            }
            if(g.high[(j+32*i)&1023]==1){
                pieceT p=g.topPiece((j+32*i)&1023);
                string s=PiecetoString(p);
                cout<<s;
                if(s.size()==2)cout<<" ";
            }            
            cout<<"|";
        }
        cout<<endl;
    }

    cout<<"HIGHT:"<<endl;
    for(int i=16;i<48;i++){
        printf("%4d",(32*i+16)&1023);
        if(i<10)cout<<" ";
        for(int j=16;j<i;j++)std::cout<<"  ";
        for(int j=16;j<48;j++){
            if(board.G.high[(j+32*i)&1023]>0)
                printf("%3d",board.G.high[(j+32*i)&1023]);
            else
                cout<<"   ";
            cout<<"|";
        }
        cout<<endl;
    }
    
    cout<<"OCCUPIED: "<<board.G.occupied.bv<<endl;
    for(int i=16;i<48;i++){
        printf("%4d",(32*i+16)&1023);
        if(i<10)cout<<" ";
        for(int j=16;j<i;j++)std::cout<<"  ";
        for(int j=16;j<48;j++){
            if(board.G.occupied.get_bit((j+32*i)&1023))
                printf(" 1 ");
            else
                cout<<"   ";
            cout<<"|";
        }
        cout<<endl;
    }

    cout<<" \n\n";
    for(int i=1;i<=28;i++){
        string s=PiecetoString(i);
        cout<<s;
        if(s.size()==2)cout<<" ";
        cout<<"|";
    }
    cout<<endl;
    for(int i=1;i<=28;i++){
        printf("%3d|",board.G.bugPosition[i]%1000);
    }
    cout<<endl;
    for(int i=1;i<=28;i++){
        printf("%3d|",board.G.isPlaced[i]?1:0);
    }
    cout<<endl;
}

/*

More info : i=609, rt=0, seed=1745452058
Run with seed:1745452058
ERRORE :std::exception
*/