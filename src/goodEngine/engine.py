from typing import TypeGuard, Final, Optional
from game_rule import DLLGameRule
from randomAgent import RandomAgentCPP as Random
from minimaxAgent import MinimaxAgentCPP as Minimax
from copy import deepcopy
from enums import *
import re

GR=DLLGameRule()

class Engine():
  VERSION: Final[str] = "1.0.0"
  """
  Engine version.
  """
  
  
    
  POSSIBLE_VALUES: Final[dict[str, list[str]]] = {
    "Strategy": ["Random", "Minimax", "DRL"],
  }
  
  

  def __init__(self) -> None:
    self.board= None
    self.brain  = Minimax()

  def start(self) -> None:
    """
    Engine main loop to handle commands.
    """
    self.info()
    while True:
      print(f"ok")
      match input().strip().split():
        case [Command.INFO]:
          self.info()
        case [Command.HELP, *arguments]:
          self.help(arguments)
        case [Command.OPTIONS, *arguments]:
          self.options(arguments)
        case [Command.NEWGAME, *arguments]:
          self.newgame(arguments)
        case [Command.VALIDMOVES]:
          self.validmoves()
        case [Command.BESTMOVE, restriction, value]:
          self.bestmove(restriction, value)
        case [Command.PLAY, move]:
          self.play(move)
        case [Command.PLAY, partial_move_1, partial_move_2]:
          self.play(f"{partial_move_1} {partial_move_2}")
        case [Command.PASS]:
          self.play("pass")
        case [Command.UNDO, *arguments]:
          self.undo(arguments)
        case [Command.EXIT]:
          print("ok")
          break
        case _:
          self.error("Invalid command. Try 'help' to see a list of valid commands and how to use them")

  def info(self) -> None:
    """
    Handles 'info' command.
    """
    print(f"id AntElligenceEngine v{self.VERSION}")
    print("Mosquito;Ladybug;Pillbug")

  def help(self, arguments: list[str]) -> None:
    """
    Handles 'help' command with arguments.

    :param arguments: Command arguments.
    :type arguments: list[str]
    """
    if arguments:
      if len(arguments) > 1:
        self.error(f"Too many arguments for command '{Command.HELP}'")
      else:
        match arguments[0]:
          case Command.INFO:
            print(f"  {Command.INFO}")
            print()
            print("  Displays the identifier string of the engine and list of its capabilities.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#info.")
          case Command.HELP:
            print(f"  {Command.HELP}")
            print(f"  {Command.HELP} [Command]")
            print()
            print("  Displays the list of available commands. If a command is specified, displays the help for that command.")
          case Command.NEWGAME:
            print(f"  {Command.NEWGAME} [GameTypeString|GameString]")
            print("")
            print("  Starts a new Base game with no expansion pieces. If GameTypeString is specified, start a game of that type. If a GameString is specified, load it as the current game.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#newgame.")
          case Command.VALIDMOVES:
            print(f"  {Command.VALIDMOVES}")
            print("")
            print("  Displays a list of every valid move in the current game.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#validmoves.")
          case Command.BESTMOVE:
            print(f"  {Command.BESTMOVE} time MaxTime")
            print(f"  {Command.BESTMOVE} depth MaxDepth")
            print("")
            print("  Search for the best move for the current game. Use 'time' to limit the search by time in hh:mm:ss or use 'depth' to limit the number of turns to look into the future.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#bestmove.")
          case Command.PLAY:
            print(f"  {Command.PLAY} MoveString")
            print("")
            print("  Plays the specified MoveString in the current game.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#play.")
          case Command.PASS:
            print(f"  {Command.PASS}")
            print("")
            print("  Plays a passing move in the current game.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#pass.")
          case Command.UNDO:
            print(f"  {Command.UNDO} [MovesToUndo]")
            print("")
            print("  Undoes the last move in the current game. If MovesToUndo is specified, undo that many moves.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#undo.")
          case Command.EXIT:
            print(f"  {Command.EXIT}")
            print("")
            print("  Exits the engine.")
          case _:
            self.error(f"Unknown command '{arguments[0]}'")
    else:
      print("Available commands:")
      for command in Command:
        print(f"  {command}")
      print(f"Try '{Command.HELP} <command>' to see help for a particular Command.")
      
  def newgame(self, arguments: list[str]) -> None:
    """
    Handles 'newgame' command with arguments.

    :param arguments: Command arguments.
    :type arguments: list[str]
    :return: The newly created Board if successful in creating a new game.
    :rtype: Optional[Board]
    """
    try:
      self.board = GR.init_state()  
      self.turnNumber=1
      self.conf=""
      self.brain=Minimax()
      self.printConf()
    except (ValueError, TypeError) as e:
      self.error(e)

  def validmoves(self) -> None:
    """
    Handles 'validmoves' command.

    :param board: Current playing Board.
    :type board: Optional[Board]
    """
    if self.is_active(self.board):
      print(GR.getAllActions(self.board))
      
      

  def bestmove(self, restriction: str, value: str) -> None:
    """
    Handles 'bestmove' command with arguments.  
    Currently WIP.

    :param board: Current playing Board.
    :type board: Optional[Board]
    :param restriction: Type of restriction in searching the best move.
    :type restriction: str
    :param value: Value of the restriction.
    :type value: str
    """
    if self.is_active(self.board):
      if restriction == "depth":
        if not value.isdigit() or not int(value) > 0:
          self.error("Invalid depth format. Use a positive integer")
          return
        self.brain.depth=(int(value))
      print(self.brain.bestmove())

  def play(self, move: str) -> None:
    """
    Handles 'play' command with its argument (MoveString).

    :param board: Current playing Board.
    :type board: Optional[Board]
    :param move: MoveString.
    :type move: str
    """
    if self.is_active(self.board):
      try:
        self.conf+=";"+str(move)
        self.turnNumber+=1
        GR.next_state(self.board,GR.stringToAction(self.board,move))
        self.brain.playmove(move)
        self.printConf()
      except ValueError as e:
        self.error(e)

  def undo(self, arguments: list[str]) -> None:
    raise "Not implemented"
    """
    Handles 'undo' command with arguments.

    :param board: Current playing Board.
    :type board: Optional[Board]
    :param arguments: Command arguments.
    :type arguments: list[str]
    """
    if self.is_active(self.board):
      if len(arguments) <= 1:
        try:
          if arguments:
            if (amount := arguments[0]).isdigit():
              self.board.undo(int(amount))
            else:
              raise ValueError(f"Expected a positive integer but got '{amount}'")
          else:
            self.board.undo()
          self.brain.empty_cache()
          print(self.board)
        except ValueError as e:
          self.error(e)
      else:
        self.error(f"Too many arguments for command '{Command.UNDO}'")

  def is_active(self, board):
    """
    Checks whether the current playing Board is initialized.

    :param board: Current playing Board.
    :type board: Optional[Board]
    :return: Whether the Board is initialized.
    :rtype: TypeGuard[Board]
    """
    if not board:
      self.error("No game is currently active")
      return False
    return True

  def error(self, error: str | Exception) -> None:
    """
    Outputs as an error the given message/exception.

    :param message: Message or exception.
    :type message: str | Exception
    """
    print(f"err {error}.")
  def printConf(self):
    ris="Base;"
    if(self.turnNumber==1):
      ris+="NotStarted;"
    else:
      ris+="InProgress;"
    if(self.turnNumber%2==1):
      ris+="White["
    else:
      ris+="Black["
    ris+=str((self.turnNumber//2))+"]"
    ris+=self.conf
    print(ris)

if __name__ == "__main__":
  Engine().start()
