#include "../engine/board.h"
#include "../engine.h"
#include <chrono>
#include <iostream>


using namespace std;

void printBoard(Board& board);

//extern bool isDBG;
int main(){
    string v[12]={"wL","bL wL\\","wQ -wL","bA1 bL-","wA1 wL/","bM bL\\","wM \\wL","bQ /bL","wA1 bM\\","bA1 -wQ","wA2 wL/","bA2 \\bA1"};
    
    Board b(GameType::Base_MLP);
    for(int i=0;i<12;i++){
        Board pb(b);
        actionT na=stringToAction(b,v[i]);
        bool seen=false;
        //if(i>4)isDBG=true;
        b.ComputePossibleMoves();

        for(int i=0;i<b.numAction;i++){
            if(stringToAction(b,actionToString(b.resAction[i],b)) != b.resAction[i]){

                cout<<"Error in transformation \n\n"<<endl;
                cout<<" Trying to execute move "<<b.resAction[i]<<" aka (b:"<<b.resAction[i]<<", pos:"<< b.resAction[i]%32<<") converted to str"<<actionToString(b.resAction[i],b)<<" to "<<stringToAction(b,actionToString(b.resAction[i],b))<<endl;
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