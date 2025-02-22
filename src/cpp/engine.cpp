#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include <regex>
#include <algorithm>
#include "engine/engine_interface.cpp"

// Forward declarations
class Strategy; //TODO: implement
class Random;
class Minimax;

// Enum for commands
enum class Command {
    INFO,
    HELP,
    OPTIONS,
    NEWGAME,
    VALIDMOVES,
    BESTMOVE,
    PLAY,
    PASS,
    UNDO,
    EXIT,
    GET,
    SET,
    UNKNOWN
};

enum class StrategyName {
    RANDOM = 0,
    MINIMAX = 1,
    DRL = 2
};

// Convert string to Command enum
Command stringToCommand(const std::string& cmd) {
    if (cmd == "info") return Command::INFO;
    if (cmd == "help") return Command::HELP;
    if (cmd == "options") return Command::OPTIONS;
    if (cmd == "newgame") return Command::NEWGAME;
    if (cmd == "validmoves") return Command::VALIDMOVES;
    if (cmd == "bestmove") return Command::BESTMOVE;
    if (cmd == "play") return Command::PLAY;
    if (cmd == "pass") return Command::PASS;
    if (cmd == "undo") return Command::UNDO;
    if (cmd == "exit") return Command::EXIT;
    if (cmd == "get") return Command::GET;
    if (cmd == "set") return Command::SET;
    return Command::UNKNOWN;
}

std::string optionToString(StrategyName brain) {
    switch (brain) {
        case StrategyName::RANDOM:
            return "Random";
        case StrategyName::MINIMAX:
            return "Minimax";
        case StrategyName::DRL:
            return "DRL";
    }
}

StrategyName stringToStrategyName(const std::string& brain) {
    if (brain == "Random") return StrategyName::RANDOM;
    if (brain == "Minimax") return StrategyName::MINIMAX;
    if (brain == "DRL") return StrategyName::DRL;
    return StrategyName::RANDOM;
}

class Engine {
private:
    const std::string VERSION = "2.0.0";
    const std::string OPTION = "Strategy";
    
    std::vector<Strategy> BRAINS;
    StrategyName current_brain = StrategyName::RANDOM;

public:
    Engine() {
        // Initialize BRAINS map
        BRAINS.push_back(new Random()); //TODO
        BRAINS.push_back(new Minimax());
    }

    void start() {
        info();
        std::string line;
        
        while (true) {
            std::cout << "ok" << std::endl;
            
            std::getline(std::cin, line);
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (iss >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.empty()) continue;
            
            try {
                Command cmd = stringToCommand(tokens[0]);
                std::vector<std::string> args(tokens.begin() + 1, tokens.end());
                
                switch (cmd) {
                    case Command::INFO:
                        info();
                        break;
                    case Command::HELP:
                        help(args);
                        break;
                    case Command::OPTIONS:
                        options(args);
                        break;
                    case Command::NEWGAME:
                        newgame(args);
                        break;
                    case Command::VALIDMOVES:
                        validmoves();
                        break;
                    case Command::BESTMOVE:
                        if (args.size() == 2) {
                            bestmove(args[0], args[1]);
                        } else {
                            error("Invalid number of arguments for bestmove");
                        }
                        break;
                    case Command::PLAY:
                        if (args.size() == 1) {
                            play(args[0]);
                        } else if (args.size() == 2) {
                            play(args[0] + " " + args[1]);
                        } else {
                            error("Invalid number of arguments for play");
                        }
                        break;
                    case Command::PASS:
                        play("pass");
                        break;
                    case Command::UNDO:
                        undo(args);
                        break;
                    case Command::EXIT:
                        std::cout << "ok" << std::endl;
                        return;
                    default:
                        error("Invalid command. Try 'help' to see a list of valid commands and how to use them");
                }
            } catch (const std::exception& e) {
                error(e.what());
            }
        }
    }

    void info() {
        std::cout << "id AntElligenceEngine v" << VERSION << std::endl;
        std::cout << "Mosquito;Ladybug;Pillbug" << std::endl;
    }

    void help(const std::vector<std::string>& arguments) {
        if (!arguments.empty()) {
            if (arguments.size() > 1) {
                error("Too many arguments for command 'help'");
                return;
            }
            
            try {
                Command cmd = stringToCommand(arguments[0]);
                // Print help for specific command
                printCommandHelp(cmd);
            } catch (const std::exception&) {
                error("Unknown command '" + arguments[0] + "'");
            }
        } else {
            std::cout << "Available commands:" << std::endl;
            // Print all available commands
            printAllCommands();
            std::cout << "Try 'help <command>' to see help for a particular Command." << std::endl;
        }
    }

    void options(const std::vector<std::string>& arguments) {
        if (!arguments.empty()) {
            try {
                Command cmd = stringToCommand(arguments[0]);
                if (cmd == Command::GET && arguments.size() == 2) {
                    std::cout << getOption(arguments[1]) << std::endl;
                } else if (cmd == Command::SET && arguments.size() == 3) {
                    setOption(arguments[1], arguments[2]);
                    std::cout << getOption(arguments[1]) << std::endl;
                } else {
                    error("Invalid options command format");
                }
            } catch (const std::exception&) {
                error("Unknown option '" + arguments[0] + "'");
            }
        } else {
            //We only have one option
            std::cout << getOption("Strategy") << std::endl;
        }
    }

    std::string getOption(const std::string& optionName) {
        if (optionName == OPTION) {
            return optionName + ";enum;" + optionToString(current_brain) + ";Random;Random;Minimax;DRL";
        }
        throw std::runtime_error("Invalid option name");
    }

    void setOption(const std::string& optionName, const std::string& optionValue) {
        if (optionName == OPTION && (optionValue == "Random" || optionValue == "Minimax" || optionValue == "DRL")) {
            current_brain = stringToStrategyName(optionValue);
        } else {
            throw std::runtime_error("Invalid option name or value");
        }
    }

    void error(const std::string& message) {
        std::cout << "err " << message << "." << std::endl;
    }

    void invalidmove(const std::string& message) {
        std::cout << "invalidmove " << message << "." << std::endl;
    }

private:
    void printCommandHelp(Command cmd) {
        // Implementation of help text for each command
        // This would contain the same text as in the Python version
        // but formatted for C++ output
    }

    void printAllCommands() {
        // Implementation to print all available commands
    }

    // Additional helper functions as needed...
};

int main() {
    Engine engine;
    engine.start();
    return 0;
}