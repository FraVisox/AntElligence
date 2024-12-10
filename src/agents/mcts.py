from random import choice
from game_logic.board import Board
from agents.strategy import Strategy

# TODO: implement monte carlo tree search
# https://ai.stackexchange.com/questions/11617/combining-deep-reinforcement-learning-with-alpha-beta-pruning
# https://dke.maastrichtuniversity.nl/m.winands/documents/paper%2049.pdf
# https://discovery.ucl.ac.uk/id/eprint/10045895/1/agz_unformatted_nature.pdf

class MCTS(Strategy):
  """
  Random acting AI agent.
  """

  def calculate_best_move(self, board: Board) -> str:
    if not self._cache:
      self._cache = choice(board.valid_moves.split(";"))
    return self._cache
