from enum import StrEnum, Flag, auto
from functools import reduce

class Command(StrEnum):
  """
  Available command.
  """
  INFO = "info"
  """
  Displays the identifier string of the engine and list of its capabilities.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#info.
  """
  HELP = "help"
  """
  Displays the list of available commands.  
  If a command is specified, displays the help for that command.
  """
  OPTIONS = "options"
  """
  Displays the available options for the engine.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#options.
  """
  NEWGAME = "newgame"
  """
  Starts a game. The game type and state depend on the command arguments.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#newgame.
  """
  VALIDMOVES = "validmoves"
  """
  Displays a list of every valid move in the current game.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#validmoves.
  """
  BESTMOVE = "bestmove"
  """
  Search for the best move for the current game.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#bestmove.
  """
  PLAY = "play"
  """
  Plays the specified MoveString in the current game.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#play.
  """
  PASS = "pass"
  """
  Plays a passing move in the current game.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#pass.
  """
  UNDO = "undo"
  """
  Undoes the specified amount of moves in the current game.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#undo.
  """
  EXIT = "exit"
  """
  Exits the engine.
  """
  GET = "get"
  """
  Displays the arguments of the specified option for the engine.  
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#options.
  """
  SET = "set"
  """
  Sets the value of the specified option to the specified value and then displays the get option result.
  See https://github.com/jonthysell/Mzinga/wiki/UniversalHiveProtocol#options.
  """

class PlayerColor(StrEnum):
  """
  Player color.
  """
  WHITE = "White"
  """
  White.
  """
  BLACK = "Black"
  """
  Black.
  """

  @property
  def code(self) -> str:
    """
    Short string representation.

    :rtype: str
    """
    return self[0].lower()

class GameState(StrEnum):
  """
  Game state.
  """
  NOT_STARTED = "NotStarted"
  """
  Game not started yet.
  """
  IN_PROGRESS = "InProgress"
  """
  Game in progress.
  """
  DRAW = "Draw"
  """
  Game ended with a draw.
  """
  WHITE_WINS = "WhiteWins"
  """
  Game ended with victory for white.
  """
  BLACK_WINS = "BlackWins"
  """
  Game ended with victory for black.
  """

  @classmethod
  def parse(cls, state: str):
    """
    Parses a GameStateString.

    :param state: GameStateString.
    :type state: str
    :return: GameState.
    :rtype: GameState
    """
    return GameState(state) if state else GameState.NOT_STARTED

class GameType(Flag):
  """
  Game type.  
  Determines the available pieces.
  """
  Base = auto()
  """
  Base GameType, without expansions.
  """
  M = auto()
  """
  Mosquito GameType, with the Mosquito expansion.
  """
  L = auto()
  """
  Ladybug GameType, with the Ladybug expansion.
  """
  P = auto()
  """
  Pillbug GameType, with the Pillbug expansion.
  """

  @classmethod
  def parse(cls, type: str):
    """
    Parses a GameTypeString.  
    The GameTypeString always needs to include the Base GameType.

    :param type: GameTypeString.
    :type type: str
    :raises ValueError: If it's not a valid GameTypeString.
    :return: GameType.
    :rtype: GameType
    """
    if type:
      base, *expansions = type.split("+")
      try:
        if GameType[base] != GameType.Base or expansions == [""] or len(expansions) > 1 and type.find("+") >= 0: raise KeyError()
        return reduce(lambda type, expansion: type | expansion, [GameType[expansion] for expansion in (expansions[0] if expansions else "")], GameType[base])
      except KeyError:
        raise ValueError(f"'{type}' is not a valid GameType")
    return GameType.Base

  def __str__(self) -> str:
    return "".join(str(gametype.name) + ("+" if gametype is GameType.Base and len(self) > 1 else "") for gametype in self)

class BugType(StrEnum):
  """
  Bug type.
  """
  QUEEN_BEE = "Q"
  """
  Queen Bee.  
  Moves by 1 cell.  
  Must be placed after the first turn and by the fourth.  
  Surrounding this piece with other pieces is the win condition.
  """
  SPIDER = "S"
  """
  Spider.  
  Moves by exactly 3 cells around the hive.
  """
  BEETLE = "B"
  """
  Beetle.  
  Moves by 1 cell, but can also go above the hive.  
  Pieces below the Beetle cannot move and the cell is now considered of the Beetle's color.  
  Can go above other pieces already above the hive, piling up.
  """
  GRASSHOPPER = "G"
  """
  Grasshopper.  
  Moves in a straight by jumping over other pieces.
  """
  SOLDIER_ANT = "A"
  """
  Soldier Ant.  
  Moves by any amount of cells around the hive.
  """
  MOSQUITO = "M"
  """
  Mosquito.  
  Available with GameType expansion M.  
  Moves by coping adiacent pieces' moveset.  
  If it goes above the hive by coping a Beetle, it can keep moving like a Beetle until it goes back down.  
  If it's only neighboring piece is another Mosquito, it cannot move.
  """
  LADYBUG = "L"
  """
  Ladybug.  
  Available with GameType expansion L.  
  Moves by exactly 3 cells, like a Spider, but the first 2 steps must be above the hive.
  """
  PILLBUG = "P"
  """
  Pillbug.  
  Available with GameType expansion P.  
  Moves by 1 cell.  
  Instead of moving itself, it can move a neighboring piece above itself, and then back down on a neighboring empty cell.
  """

class Direction(StrEnum):
  """
  Hexagonal multi-layered grid direction.  
  The grid is assumed to be oriented with cells point-up.
  """
  RIGHT = "|-"
  """
  Right side of the hex: bug-
  """
  UP_RIGHT = "|/"
  """
  Up right side of the hex: bug/
  """
  UP_LEFT = "\\|"
  """
  Up left side of the hex: \\bug
  """
  LEFT = "-|"
  """
  Left side of the hex: -bug
  """
  DOWN_LEFT = "/|"
  """
  Down left side of the hex: /bug
  """
  DOWN_RIGHT = "|\\"
  """
  Down right side of the hex: bug\\
  """
  BELOW = ""
  """
  Below of the hex: described by another of the other direction indications.
  """
  ABOVE = "|"
  """
  Above of the hex: bug
  """

  @classmethod
  def flat(cls):
    """
    Returns all flat directions (no directions to move up or down a plane). 

    :return: List of flat directions.
    :rtype: list[Direction]
    """
    return [direction for direction in Direction if direction is not Direction.ABOVE and direction is not Direction.BELOW]
  
  @classmethod
  def flat_left(cls):
    """
    Returns all flat left directions. 

    :return: List of flat left directions.
    :rtype: list[Direction]
    """
    return [direction for direction in Direction if direction.is_left]

  @classmethod
  def flat_right(cls):
    """
    Returns all flat right directions. 

    :return: List of flat right directions.
    :rtype: list[Direction]
    """
    return [direction for direction in Direction if direction.is_right]

  def __str__(self) -> str:
    return self.replace("|", "")

  @property
  def opposite(self):
    """
    Opposite direction.

    :rtype: Direction
    """
    match self:
      case Direction.BELOW | Direction.ABOVE:
        return list(Direction)[(self.delta_index - 5) % 2 + 6]
      case _:
        return list(Direction)[(self.delta_index + 3) % 6]

  @property
  def left_of(self):
    """
    Direction to the left.

    :rtype: Direction
    """
    match self:
      case Direction.BELOW | Direction.ABOVE:
        return self
      case _:
        return list(Direction)[(self.delta_index + 1) % 6]
  
  @property
  def right_of(self):
    """
    Direction to the right.

    :rtype: Direction
    """
    match self:
      case Direction.BELOW | Direction.ABOVE:
        return self
      case _:
        return list(Direction)[(self.delta_index + 5) % 6]

  @property
  def delta_index(self) -> int:
    """
    Direction index.

    :rtype: int
    """
    return list(Direction).index(self)

  @property
  def is_right(self) -> bool:
    """
    Whether it's a right direction.

    :rtype: bool
    """
    return self is Direction.RIGHT or self is Direction.UP_RIGHT or self is Direction.DOWN_RIGHT

  @property
  def is_left(self) -> bool:
    """
    Whether it's a left direction.

    :rtype: bool
    """
    return self is Direction.LEFT or self is Direction.UP_LEFT or self is Direction.DOWN_LEFT
