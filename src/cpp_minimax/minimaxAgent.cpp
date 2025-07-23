#include "cpp_minimax/minimaxAgent.h"

MinimaxAgentCPP::MinimaxAgentCPP(int typeGame, double* weights, int depth, const std::vector<std::string>& listmoves)
    : gametype(typeGame), depth(depth), currentTurn(1), numEval(0), name("MINIMAX"), gameState("NotStarted") {
    w = weights ? weights : new double[6]{0, 0, 0, 1, -2, 30};
    state = GR.init_state(typeGame);
    gameLog = "";
    gameType = encodeGamemode(typeGame);
    
    for (const auto& move : listmoves) {
        playmove(move);
    }
}

std::string MinimaxAgentCPP::encodeGamemode(int gamemode) {
    switch (gamemode) {
        case 0: return "Base";
        case 1: return "Base+M";
        case 2: return "Base+L";
        case 3: return "Base+P";
        case 4: return "Base+ML";
        case 5: return "Base+MP";
        case 6: return "Base+LP";
        case 7: return "Base+MLP";
        default: return "Unknown";
    }
}

void MinimaxAgentCPP::undo(int amount) {
    if (amount <= 0) return;

    std::vector<std::string> moves;
    std::stringstream ss(gameLog);
    std::string segment;
    while (std::getline(ss, segment, ';')) {
        if (!segment.empty()) moves.push_back(segment);
    }

    reset();
    for (size_t i = 3; i < moves.size() - amount; ++i) {
        playmove(moves[i]);
    }
}

std::string MinimaxAgentCPP::gameinfo() {
    std::string player = (currentTurn % 2 == 0) ? "White" : "Black";
    return gameType + ";" + gameState + ";" + player + "[" + std::to_string(currentTurn / 2) + "]" + gameLog;
}

std::vector<std::string> MinimaxAgentCPP::validmoves() {
    return GR.getActions(state);
}

void MinimaxAgentCPP::set_time_limit(int seconds) {
    // Not implemented
}

void MinimaxAgentCPP::set_depth_limit(int d) {
    depth = d;
}

std::string MinimaxAgentCPP::bestmove() {
    return GR.actionToString(bestaction(), state);
}

void MinimaxAgentCPP::playmove(const std::string& actionStr) {
    std::cout << "Decoding action..." << std::endl;
    auto action = GR.stringToAction(state, actionStr);
    std::cout << "Decoded action" << std::endl;
    executeAction(action);
    gameLog += ";" + actionStr;
}

void MinimaxAgentCPP::executeAction(const std::string& action) {
    int status = GR.checkStatus(state);
    if (status > 1) return;

    GR.next_state(state, action);
    currentTurn++;

    switch (GR.checkStatus(state)) {
        case 1: gameState = "InProgress"; break;
        case 2: gameState = "WhiteWins"; break;
        case 3: gameState = "BlackWins"; break;
        case 4: gameState = "Draw"; break;
    }
}

std::string MinimaxAgentCPP::bestaction() {
    numEval = 0;
    auto [val, action] = minimax(state, depth, true, w, -1000, 1000, currentTurn);
    return action;
}

void MinimaxAgentCPP::reset() {
    currentTurn = 0;
    gameLog = "";
    gameState = "NotStarted";
    GR.delBoard(state);
    state = GR.init_state(gametype);
}

void MinimaxAgentCPP::delGame() {
    currentTurn = 0;
    GR.delBoard(state);
}

std::pair<int, std::string> MinimaxAgentCPP::minimax(void* state, int depth, bool maximizingPlayer, double* w, int alpha, int beta, int currentTurn) {
    if (GR.checkStatus(state) != 1) {
        return maximizingPlayer ? std::make_pair(-1000, "") : std::make_pair(1000, "");
    }

    if (depth == 0) {
        numEval++;
        return { GR.calcVal(state, w), "" };
    }

    std::vector<std::string> actions = GR.getActions(state);
    if (actions.empty()) return { maximizingPlayer ? -1000 : 1000, "" };

    std::string bestAction = actions[0];

    if (maximizingPlayer) {
        int maxEval = -1000;
        for (const auto& action : actions) {
            void* child = GR.copyBoard(state);
            GR.next_state(child, action);
            auto [evalV, _] = minimax(child, depth - 1, false, w, alpha, beta, currentTurn + 1);
            GR.delBoard(child);

            if (evalV > maxEval) {
                maxEval = evalV;
                bestAction = action;
            }
            alpha = std::max(alpha, evalV);
            if (beta <= alpha) break;
        }
        return { maxEval, bestAction };
    } else {
        int minEval = 1000;
        for (const auto& action : actions) {
            void* child = GR.copyBoard(state);
            GR.next_state(child, action);
            auto [evalV, _] = minimax(child, depth - 1, true, w, alpha, beta, currentTurn + 1);
            GR.delBoard(child);

            if (evalV < minEval) {
                minEval = evalV;
                bestAction = action;
            }
            beta = std::min(beta, evalV);
            if (beta <= alpha) break;
        }
        return { minEval, bestAction };
    }
}
