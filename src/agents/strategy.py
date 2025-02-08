from typing import Optional
from abc import ABC, abstractmethod
from engineInterface import EngineDLL
class Strategy(ABC):
  """
  Base abstract class for AI agents.
  """
  time_limit = 0
  depth_limit = 0
  
  DISABLE_CACHE = True

  def __init__(self) -> None:
    self._cache: Optional[str] = None

  @abstractmethod
  def calculate_best_move(self, engineInterface: EngineDLL) -> str:
    pass

  def empty_cache(self) -> None:
    self._cache = None
    
  def set_time_limit(self, time: int):
    self.time_limit = time
    
  def set_depth_limit(self, depth: int):
    self.depth_limit = depth

