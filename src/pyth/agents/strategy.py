from typing import Optional
from abc import ABC, abstractmethod
from game_logic.board import Board

class Strategy(ABC):
  """
  Base abstract class for AI agents.
  """
  time_limit = 0
  depth_limit = 0
  
  DISABLE_CACHE = True

  def __init__(self) -> None:
    self._cache: Optional[str] = None

  @abstractmethod # TODO: what to pass to it?
  def calculate_best_move(self, board: Board) -> str:
    pass

  def empty_cache(self) -> None:
    self._cache = None
    
  def set_time_limit(self, time: int):
    self.time_limit = time
    
  def set_depth_limit(self, depth: int):
    self.depth_limit = depth

