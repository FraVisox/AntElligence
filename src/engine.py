from typing import Final, Optional
from game_logic_py.enums import Command
from agents.strategy import Strategy
from agents.random_strat import Random
#from engine_cpp.agents.minimax import Minimax
from copy import deepcopy
#import engineInterface as engineInterface
from engineInterface import engine as CPPInterface
from BoardModel import BoardModel
import re


class Engine():
  VERSION: Final[str] = "2.0.0"
  """
  Engine version.
  """
  
  OPTIONS: Final[dict[str, str]] = {
    "Strategy": "Random",
  }
  
    
  POSSIBLE_VALUES: Final[dict[str, list[str]]] = {
    "Strategy": ["Random", "Minimax", "DRL"],
  }
  
  BRAINS: Final[dict[str, Strategy]] = {
    "Random": Random(),
#    "Minimax": Minimax(),
  }



  def __init__(self) -> None:
    self.brain: Strategy = self.BRAINS[self.OPTIONS["Strategy"]]
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
          case Command.OPTIONS:
            print(f"  {Command.OPTIONS}")
            print(f"  {Command.OPTIONS} get OptionName")
            print(f"  {Command.OPTIONS} set OptionName OptionValue")
            print("")
            print("  Displays the available options for the engine. Use 'get' to get the specified OptionName or 'set' to set the specified OptionName to OptionValue.")
            print("  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#options.")
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
      
  def options(self, arguments: list[str]) -> None:
    """
    Handles 'options' argument to change AI strategy.
    """
    if arguments:
      match arguments[0]:
        case Command.GET:
          print(self.get_option(arguments[1]))
        case Command.SET:
          self.set_option(arguments[1], arguments[2])
          print(self.get_option(arguments[1]))
        case _:
          self.error(f"Unknown option '{arguments[0]}'")
    else:
      for option in self.OPTIONS:
        print(self.get_option(option))
  
  def get_option(self, option_name):
    """
    Handles 'options get' argument to display AI strategy. As this is the only option, there is no need to make it more general.
    """
    if option_name in self.OPTIONS:
      return "{};enum;{};{};{}".format(option_name, self.OPTIONS[option_name], self.POSSIBLE_VALUES[option_name][0], ";".join(map(str, self.POSSIBLE_VALUES[option_name])))
    
  def set_option(self, option_name, option_value):
    """
    Handles 'options set' argument to change AI strategy.
    """
    if option_name in self.OPTIONS and option_value in self.POSSIBLE_VALUES[option_name]: 
      self.brain = self.BRAINS[option_value]
      self.OPTIONS[option_name] = option_value
      

  def newgame(self, arguments: list[str]) -> None:
    """
    Handles 'newgame' command with arguments.

    :param arguments: Command arguments.
    :type arguments: list[str]
    :return: The newly created Board if successful in creating a new game.
    :rtype: Optional[Board]
    """
    try:
      if not CPPInterface.start_game(" ".join(arguments)):
        self.error("Failed to start the game as the string was invalid")
      else: 
        print(CPPInterface.get_board())
    except (ValueError, TypeError) as e:
      self.error(e)

  def validmoves(self) -> None:
    """
    Handles 'validmoves' command.

    :param board: Current playing Board.
    :type board: Optional[Board]
    """
    print(CPPInterface.get_valid_moves())
      
  def parse_time_to_seconds(self, time_str):
    # Split the time string by colon
    hours, minutes, seconds = map(int, time_str.split(":"))
    
    # Calculate the total seconds
    total_seconds = hours * 3600 + minutes * 60 + seconds
    
    return total_seconds

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
    if restriction == "time":
      if not re.match(r"^\d{2}:\d{2}:\d{2}$", value):
        self.error("Invalid time format. Use hh:mm:ss")
        return
      self.brain.set_time_limit(self.parse_time_to_seconds(value))
    elif restriction == "depth":
      if not value.isdigit() or not int(value) > 0:
        self.error("Invalid depth format. Use a positive integer")
        return
      self.brain.set_depth_limit(int(value))

    # TODO: how to pass the board to the brain??? This should return a MoveString
    print(self.brain.calculate_best_move(CPPInterface))

  def play(self, move: str) -> None:
    """
    Handles 'play' command with its argument (MoveString).

    :param board: Current playing Board.
    :type board: Optional[Board]
    :param move: MoveString.
    :type move: str
    """
    try:
      ret = CPPInterface.play_move(move)
      #if ret == CPPInterface.ReturnTypes.ERROR:
      #  self.error("Invalid move")
      #  return
      # TODO: make something different if someone wins??
      self.brain.empty_cache()
      print(CPPInterface.get_board())
    except ValueError as e:
      self.invalidmove(e)

  def undo(self, arguments: list[str]) -> None:
    """
    Handles 'undo' command with arguments.

    :param board: Current playing Board.
    :type board: Optional[Board]
    :param arguments: Command arguments.
    :type arguments: list[str]
    """
    if len(arguments) <= 1:
      try:
        if arguments:
          if (amount := arguments[0]).isdigit():
            CPPInterface.undo(int(amount))
          else:
            raise ValueError(f"Expected a positive integer but got '{amount}'")
        else:
          CPPInterface.undo(1)
        self.brain.empty_cache()
        print(CPPInterface.get_board())
      except ValueError as e:
        self.error(e)
    else:
      self.error(f"Too many arguments for command '{Command.UNDO}'")

  def error(self, error: str | Exception) -> None:
    """
    Outputs as an error the given message/exception.

    :param message: Message or exception.
    :type message: str | Exception
    """
    print(f"err {error}.")


  def invalidmove(self, error: str | Exception) -> None:
    """
    Outputs the invalid move error.

    :param message: Message or exception.
    :type message: str | Exception
    """
    print(f"invalidmove {error}.")

if __name__ == "__main__":
  Engine().start()
