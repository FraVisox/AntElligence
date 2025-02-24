#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include <regex>
#include <algorithm>
#include "engine/engine_interface.h"
#include "engine/enums.h"
#include "strategy.h"

class Engine {
private:
    const std::string VERSION = "2.0.0";
    const std::string OPTION = "Strategy";
    
    Strategy BRAINS[3] = {Strategy(StrategyName::RANDOM), Strategy(StrategyName::MINIMAX), Strategy(StrategyName::DRL)};
    StrategyName current_brain = StrategyName::RANDOM;

public:
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

    void bestmove(string s1, string s2) {
        if (s1 == "time") {

        } else if (s2 == "depth") {

        }
        //TODO: complete
    }

    void play(string s) {

    }

    void undo(vector<string> numbers) {
        if (numbers.size() != 1) {
            return;
        }
        int n = std::stoi(numbers[0]);
        undoB(n);
        cout << getBoard() << endl;
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


    void newgame(std::vector<std::string> args) {
        char* s = new char[args[0].length() + 1]; //TODO: migliora
        strcpy(s, args[0].c_str());
        startGame(s);
        cout << getBoard() << endl;
    }

    void validmoves() {
        char* s = validMoves();
        std::cout << s << std::endl;
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


    void printCommandHelp(Command cmd) {
        // Implementation of help text for each command
        // This would contain the same text as in the Python version
        // but formatted for C++ output
        //TODO: complete
    }

    void printAllCommands() {
        // Implementation to print all available commands
    }

};

int main() {
    Engine engine;
    engine.start();
    return 0;
}