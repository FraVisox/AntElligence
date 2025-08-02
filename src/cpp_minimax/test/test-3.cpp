#include "../engine/board.h"
#include "../engine.h"
#include <chrono>
#include <iostream>


using namespace std;

void printBoard(Board& board);

int main(){
    string v[167]={"wG1","bS1 -wG1","wA1 wG1/","bB1 \\bS1","wL wG1\\","bB2 /bS1","wQ wA1/","bQ /bB2","wP \\wQ","bA1 -bB1","wP -wQ","bA1 -wL","wA2 wL-","bG1 bQ\\","wA3 \\wA2","bA1 -bQ","wM wA2/","bS2 -bB2","wA2 bG1\\","bL /bA1","wA2 \\wM","bG2 /bG1","wA2 /bL","bM bG1/","wG2 \\wQ","bS2 -bL","wB1 wQ-","bG3 \\bB1","wG2 \\wM","bP -bB1","wM wB1-","bA2 \\bA1","wG2 bM-","bA2 bG3/","wA2 bA2/","bA3 -bA2","wB2 \\wM","bP -bG3","wG3 \\wQ","bA3 wL-","wG1 -wB2","bA3 bG2-","wA2 -bA1","bA3 -wA2","wB2 wG1","bL \\bA1","wM wB1","bL \\wA2","wS1 \\wG3","bA2 \\wB2","wM wQ","bA2 wB1/","wM wB2","bA3 \\bA1","wS2 wB1\\","bA3 wG2-","wQ -wA1","bA2 -bP","wA3 bL/","bA2 -bL","wA1 wS2-","bA2 -bG2","wA3 bA3-","bA2 wA3\\","wA1 \\bA1","bA2 \\bG3","wA3 bA2/","bA3 wA3/","wA1 -bG2","bA3 -bP","wA3 /wS2","bA2 -bL","wA3 wG2-","bA3 \\bG3","wL \\wG2","bM -bB2","wA3 -bS1","bA2 bL/","wA1 \\wS1","bM -wA1","wA3 \\wA1","bA3 wM/","wA3 bG2-","bM bA2/","wA1 bS2\\","bA3 wS2\\","wA1 -bA3","bA3 -bG1","wA1 -bS2","bG3 wG2\\","wA1 -wS2","bA3 -bM","wA3 -bP","bA3 bG2-","wA3 bG3\\","bA3 -bG1","wM wG3","bA3 -wP","wA3 -bG2","bA3 bG3/","wA1 wB2/","bA3 wS1/","wA1 \\wB2","bA3 \\bP","wS1 \\bB1","bM -wA1","wB2 wM","bA2 -bS1","wA3 wG2/","bM wS2/","wA1 wS2\\","bA3 wA3-","wA1 -bL","bA2 -wA2","wA1 \\bP","bA2 bG3/","wA1 bG2-","bA3 \\wS1","wA3 \\bA1","bA2 \\wG1","wA3 bG3/","bA2 -wP","wA1 -bL","bG2 -wG2","wS2 wQ-","bA2 wB1\\","wA1 wA3-","bA3 bS2-","wB2 \\wM","bA3 -bL","wS2 -wM","bG2 wA1-","wM wG1","bA3 bA2-","wM -wB1","bA3 wA1\\","wS2 wQ-","bA2 bS2-","wS2 bM\\","bA2 \\wG1","wG1 -wG3","bA3 \\wA3","wG1 wQ-","bA2 -bG1","wG1 -wP","bA2 bS2-","wQ wP\\","bA2 bG2-","wQ bS1/","bA2 bG2\\","wG1 -wG3","bA2 /bG3","wG1 \\wB1","bL -wA2","wM wS2/","bA3 -wP","wS2 -wB1","bA3 bM\\","wM -bP","bA3 wB1\\","wG1 bA3\\","bA2 -bL","wG1 \\wB1","bM -wB2","wS2 -wP","bA2 bS2\\","wM \\bP"};
    
    Board b(GameType::Base_MLP);
    for(int i=0;i<167;i++){
        Board pb(b);
        actionT na=stringToAction(b,v[i]);
        bool seen=false;
        b.ComputePossibleMoves();

        for(int i=0;i<b.numAction;i++){
            if(stringToAction(b,actionToString(b.resAction[i],b)) != b.resAction[i]){
                cout<<"Error in transformation \n\n"<<endl;

                pb.printBoard();
                cout<<" \n\n";

                b.printBoard();
            }
            if(na==b.resAction[i])seen=true;
        }
        if(!seen){
            cout<<"Impossible to find the action "<<v[i]<<" in board n "<<i <<endl;
            b.printBoard();
            return 0;
        }
        cout<<"Ok "<<i<<" : "<<actionToString(na,b)<<endl;
        b.applayAction(na);

    }
    MinimaxAgent m(5);
    cout<<"Created agent"<<endl;
    b.ComputePossibleMoves();
    cout<<"Computed next: "<< b.numAction<<endl;
    for(int i=0;i<b.numAction;i++){
        cout<<actionToString(b.resAction[i],b)<<" , ";
    }
    cout<<endl;
    for(int i=0;i<b.numAction;i++){
        if(stringToAction(b,actionToString(b.resAction[i],b)) != b.resAction[i]){
            cout<<"Error in transformation \n\n"<<endl;
        }
    }
    clog<<"Just act"<<endl;

    //actionT bm=m.calculate_best_move(b);
    b.printBoard();
    //cout<<actionToString(bm,b);
    cout<<endl;
    cout<<"Computed next"<< b.numAction<<endl;
    for(int i=0;i<b.numAction;i++){
        cout<<actionToString(b.resAction[i],b)<<" , ";
    }
    return 0;
}