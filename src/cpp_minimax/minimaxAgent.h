#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include "cpp_minimax/game_rule_reduced.cpp"

class MinimaxAgentCPP {
private:
    DLLGameRule GR;
    double* w;
    int currentTurn;
    int depth;
    int gametype;
    int numEval;
    void* state;
    std::string name;
    std::string gameState;
    std::string gameLog;
    std::string gameType;

    std::string encodeGamemode(int gamemode);

    std::pair<int, std::string> minimax(void* state, int depth, bool maximizingPlayer,
                                        double* w, int alpha = -1000, int beta = 1000, int currentTurn = 0);

public:
    MinimaxAgentCPP(int typeGame = 0, double* weights = nullptr, int depth = 4, const std::vector<std::string>& listmoves = {});

    void undo(int amount);
    std::string gameinfo();
    std::vector<std::string> validmoves();
    void set_time_limit(int seconds);
    void set_depth_limit(int d);
    std::string bestmove();
    void playmove(const std::string& actionStr);
    void executeAction(const std::string& action);
    std::string bestaction();
    void reset();
    void delGame();
};
