#ifndef AGENT_H
#define AGENT_H
#include "../engine/board.h"
using namespace std;

class Agent{
    public:
    int time_limit=0;
    int depth_limit=0;


    Agent(int tl=100000,int dl=1000){
        time_limit=tl;
        depth_limit=dl;
    }

    virtual action calculate_best_move(Board&)=0;
    void set_depth_limit(int d){
        depth_limit=d;
    }
    void set_time_limit(int t){
        time_limit=t;
    }
    virtual ~Agent() {} // Virtual destructor

};

#endif