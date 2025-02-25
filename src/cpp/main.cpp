#include <iostream>
#include "engine.h"
#include "agents/random.h"

int main(){

    RandomAgent a;

    Engine e(a);

    e.start();
    return 0;
}