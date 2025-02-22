'''
from random import choice
from game_logic_py.board import Board
from agents.strategy import Strategy

# TODO: implement monte carlo tree search
# https://ai.stackexchange.com/questions/11617/combining-deep-reinforcement-learning-with-alpha-beta-pruning
# https://dke.maastrichtuniversity.nl/m.winands/documents/paper%2049.pdf
# https://discovery.ucl.ac.uk/id/eprint/10045895/1/agz_unformatted_nature.pdf


# Idea: we don't need to explore all the moves of the player
#       we want only to randomly chose a portion
#         if we are on the i level, we will chose to analyze a move w.p. $(P/M)^i$ with P fixed and
#       the E of moves is constant and depends only on p
class MCTS(Strategy):
  """
  Random acting AI agent.
  """

  def calculate_best_move(self, board: Board) -> str:
    if not self._cache:
      self._cache = choice(board.valid_moves.split(";"))
    return self._cache
'''
