#ifndef ENGINE_H
#define ENGINE_H
#include <string>
#include <iostream>
#include "agents/agent.h"
using namespace std;


class Engine{
    public:
    //  Engine version
    string VERSION="3.0";

    EBoard* board;

    // Agent
    Agent& agent;
    
    Engine(Agent& a):agent(a){}

  void start(){
    //Engine main loop to handle commands.
    info("");
    while(true){
      cout<<"ok\n";
      string input;
      getline(cin,input);
      int d=input.find(' ');

      string command,arg;
      if(d==-1){
        command=input;
        arg="";
      }else{
        command=input.substr(0,d);
        arg=input.substr(d+1);
      }
      
      if(command=="info")
          info(arg);
      else if(command=="help")
          help(arg);
      else if(command=="options")
          options(arg);
      else if(command=="newgame")
        newgame(arg);
      else if(command=="validmoves")
        validmoves();
      else if(command=="bestmove")
          bestmove(arg);
      else if(command=="play")
          play(arg);
      else if(command=="pass")
          play("pass");
        else if(command=="undo") 
          undo(arg);
        else if(command=="exit") {
          cout<<"ok"<<endl;
          break;
        }
        else
          error("Invalid command. Try 'help' to see a list of valid commands and how to use them");
    }
  }
  void info(string arg){
    cout<<"id AntElligenceEngine "<<VERSION<<endl;
    cout<<"Mosquito;Ladybug;Pillbug"<<endl;
  }

  void help(string arg){
    if(arg.find(" "))
        error("Too many arguments for command "+arg);
    else if(arg=="info"){
            cout<<"  "<<arg<<endl<<endl;
            cout<<"  Displays the identifier string of the engine and list of its capabilities.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#info.\n";
        }else if(arg=="help"){
            cout<<"  help\n   help [command]"<<endl<<endl;
            cout<<"  Displays the list of available commands. If a command is specified, displays the help for that command."<<endl;
        }else if(arg=="options"){
            cout<<"  options"<<endl;
            cout<<"  options get OptionName"<<endl;
            cout<<"  options set OptionName OptionValue"<<endl;
            cout<<"\n";
            cout<<"  Displays the available options for the engine. Use 'get' to get the specified OptionName or 'set' to set the specified OptionName to OptionValue.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#options.\n";
      }else if(arg=="newgame"){ 
            cout<<"  newgame [GameTypeString|GameString]\n\n";
            cout<<"  Starts a new Base game with no expansion pieces. If GameTypeString is specified, start a game of that type. If a GameString is specified, load it as the current game.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#newgame.\n";
          }else if(arg=="validmoves"){
            cout<<"  validmoves\n\n";
            cout<<"  Displays a list of every valid move in the current game.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#validmoves.\n";
          }else if(arg=="bestmove"){
            cout<<"  bestmove time MaxTime\n";
            cout<<"  bestmove depth MaxDepth\n\n";
            cout<<"  Search for the best move for the current game. Use 'time' to limit the search by time in hh:mm:ss or use 'depth' to limit the number of turns to look into the future.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#bestmove.\n";
          }else if(arg=="play"){
            cout<<"  play MoveString\n\n";
            cout<<"  Plays the specified MoveString in the current game.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#play.\n";
          }else if(arg=="pass"){
            cout<<"  pass\n\n";
            cout<<"  Plays a passing move in the current game.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#pass.\n";
          }else if(arg=="undo"){
            cout<<"  undo [MovesToUndo]\n\n";
            cout<<"  Undoes the last move in the current game. If MovesToUndo is specified, undo that many moves.\n";
            cout<<"  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#undo.\n";
          }else if(arg=="exit"){
            cout<<"  exit\n\n";
            cout<<"  Exits the engine.\n";
          }
    else{
      cout<<"Available commands:\n";
      cout<<"  info\n  help\n  options\n  newgame\n  validmoves  \n  bestmove\n  play\n  pass\n  undo\n  exit\n\n";
      cout<<"Try help <command>' to see help for a particular Command.\n";
    }
  }

  void options(string arguments){
    error("Not available options");
    /*
    if (!arguments.empty()) {
        if (arguments == "get") {
          get_option(arguments);
        } else if (arguments == "set") {
          set_option(arguments);
        }
        error("Unknown option " + arguments[0]);
    } else {
      cout << "Not available options\n";
    }
    */
  }
  void get_option(string arguments){
  }

  void set_option(string arguments){
  }


  void newgame( string arguments);   
  void validmoves();
  void bestmove(string);
  void play(string);
  void undo(string arguments);


   void error(string  error){
    cout<<"err "<<error << "\n";
  }

  void invalidmove(string error){
    cout<<"invalidmove "<<error << "\n";
  }
};

#endif