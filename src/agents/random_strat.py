from random import choice
from BoardModel import BoardModel
from agents.strategy import Strategy
from engineInterface import EngineDLL

class Random(Strategy):
  """
  Random acting AI agent.
  """


  def calculate_best_move(self, board: BoardModel) -> str:
    if not self._cache or self.DISABLE_CACHE:
      self._cache = choice(board.valid_moves().split(";"))
    return self._cache
