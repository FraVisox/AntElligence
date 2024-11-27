from random import choice
from game_logic.board import Board
from agents.strategy import Strategy

# TODO: implement minimax

class Minimax(Strategy):
  """
  AI agent following a custom minimax with alpha-beta pruning policy.
  """

  def calculate_best_move(self, board: Board) -> str:
    if not self._cache:
      self._cache = choice(board.valid_moves.split(";"))
    return self._cache