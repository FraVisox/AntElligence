#ifndef MINIMAX_H
#define MINIMAX_H
#include "../engine_cpp/board.h"
using namespace std;

class Minimax{
    public:
    int time_limit=0;
    int depth_limit=0;



    virtual action calculate_best_move(Board& board){
            
    }


    void set_depth_limit(int d){
        depth_limit=d;
    }
    void set_time_limit(int t){
        time_limit=t;
    }
    virtual ~Minimax() {} // Virtual destructor

};

#endif