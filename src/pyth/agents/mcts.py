from random import choice
from game_logic.board import Board
from agents.strategy import Strategy

class MCTS(Strategy): #TODO: adapt this to MCTS (see AlphaZero)
  """
  Random acting AI agent.
  """

  def calculate_best_move(self, board: Board) -> str:
    if not self._cache:
      self._cache = choice(board.valid_moves.split(";"))
    return self._cache
