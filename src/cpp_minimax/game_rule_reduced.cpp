// DLLGameRule.h
#pragma once

#include "cpp_minimax/cpp/interface_reduced.cpp"
#include <vector>

class DLLGameRule {
public:
    static constexpr int MAX_ACTIONS = 1575;

    DLLGameRule() = default;
    ~DLLGameRule() = default;

    // Create initial state
    EBoard* init_state(int game_type = 7) {
        return base_state(game_type);
    }

    // Apply an action to the state
    void next_state(EBoard* state, actionT action) {
        ::next_state(state, action);
    }

    // Copy a board
    EBoard* copyBoard(EBoard* state) {
        return ::copyBoard(state);
    }

    // Check the status of the game
    int checkStatus(EBoard* state) {
        return ::checkStatus(state);
    }

    // Print the board
    void PrintState(EBoard* state) {
        ::printBoard(state);
    }

    // Delete a board
    void delBoard(EBoard* state) {
        ::delBoard(state);
    }

    // Convert string to action
    actionT stringToAction(EBoard* state, const std::string& str) {
        return ::stringToAction(state, const_cast<char*>(str.c_str()));
    }

    // Convert action to string
    std::string actionToString(actionT action, EBoard* state) {
        char buffer[30];
        ::actionToString(action, state, buffer);
        return std::string(buffer);
    }

    // Get valid action mask
    std::vector<bool> get_mask(EBoard* state) {
        uint8_t* mask = static_cast<uint8_t*>(::getMask(state));
        return std::vector<bool>(mask, mask + MAX_ACTIONS);
    }

    // Get associated actions
    std::vector<actionT> get_actions(EBoard* state) {
        actionT* actions = static_cast<actionT*>(::getAssociatedAction(state));
        return std::vector<actionT>(actions, actions + MAX_ACTIONS);
    }

    // Compute possible actions
    void compute_actions(EBoard* state) {
        ::updatePossiblemoves(state);
    }

    // Get actions via direct pointer output
    std::vector<actionT> getActions(EBoard* state) {
        actionT actions[MAX_ACTIONS] = {};
        ::getActions(state, actions);
        return std::vector<actionT>(actions, actions + MAX_ACTIONS);
    }

    // Evaluate board
    double calcVal(EBoard* state) {
        return ::boardEval(state);
    }
};
