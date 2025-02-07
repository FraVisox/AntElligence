from typing import Final, Optional, Set
from engine_py.enums import GameType, GameState, PlayerColor, BugType, Direction
from engine_py.game import Position, Bug, Move
from abc import ABC, abstractmethod

class BoardModel(ABC):
  
  def __init__(self, gamestring: str = "") -> None:
    pass

  
  def current_player_color(self) -> int:
    return self.current_player_turn()%2
  
  @abstractmethod
  def current_player_turn(self) -> int:
    pass

  @abstractmethod
  def current_player_queen_in_play(self) -> bool:
    """
    Whether the current player's queen bee is in play.

    :rtype: bool
    """
    pass

  @abstractmethod
  def valid_moves(self) -> str:
    """
    Current possible legal moves in a joined list of MoveStrings.

    :rtype: str
    """
    pass

  @abstractmethod
  def play(self, move_string: str) -> None:
    """
    Plays the given move.

    :param move_string: MoveString of the move to play.
    :type move_string: str
    :raises ValueError: If the game is over.
    """
    pass
  @abstractmethod
  def undo(self, amount: int = 1) -> None:
    """
    Undoes the specified amount of moves.

    :param amount: Amount of moves to undo, defaults to 1.
    :type amount: int, optional
    :raises ValueError: If there are not enough moves to undo.
    :raises ValueError: If the game has yet to begin.
    """
    pass
  @abstractmethod
  def get_oracle_eval(self) -> int:
    """
    Returns the number of bugs near the queen of the given color.

    :param color: Color of the queen.
    :type color: PlayerColor
    :return: Number of bugs near the queen.
    :rtype: int
    """
    pass
    return 0#sum(1 for bug in self._bugs_from_pos(self._pos_from_bug(Bug(color, BugType.QUEEN_BEE))) if self._is_bug_on_top(bug))
