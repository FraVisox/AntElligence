from random import choice
from game_logic.board import Board
from agents.strategy import Strategy
from game_logic.enums import *
from copy import deepcopy


class Minimax(Strategy):
  """
  AI agent following a custom minimax with alpha-beta pruning policy.
  """

  color = PlayerColor.WHITE

  def utility(self, state, board):
    # Cases where win or lose is defined
    if state == GameState.DRAW:
        return 0
    if self.color == PlayerColor.WHITE:
      if state == GameState.WHITE_WINS:
        return 100
      if state == GameState.BLACK_WINS:
        return -100
      # Calculate other utility
      return board._get_number_of_bugs_near_queen(PlayerColor.WHITE) - board._get_number_of_bugs_near_queen(PlayerColor.BLACK)
      
    
    if state == GameState.BLACK_WINS:
      return 100
    elif state == GameState.WHITE_WINS:
      return -100
    return board._get_number_of_bugs_near_queen(PlayerColor.BLACK) - board._get_number_of_bugs_near_queen(PlayerColor.WHITE)

  def minmax(self, state, board, depth, maximizing_player, alpha, beta):
    if state == GameState.DRAW or state == GameState.WHITE_WINS or state == GameState.BLACK_WINS or depth == self.depth_limit:
        return self.utility(state, board)
    
    if maximizing_player:
        max_eval = -float('inf')
        for action in board.valid_moves.split(";"):
            bb = deepcopy(board)
            bb.play(action)
            eval = self.minmax(bb.state, bb, depth + 1, False, alpha, beta)
            alpha = max(alpha, eval)
            if beta <= alpha:
                break  # Beta cut-off
            if eval > max_eval and depth == 0:
              self._cache = action
            max_eval = max(max_eval, eval)
        return max_eval
    else:
        min_eval = float('inf')
        for action in board.valid_moves.split(";"):
            bb = deepcopy(board)
            bb.play(action)
            eval = self.minmax(bb.state, bb, depth + 1, True, alpha, beta)
            beta = min(beta, eval)
            if beta <= alpha:
                break  # Alpha cut-off
            min_eval = min(min_eval, eval)
        return min_eval

  def calculate_best_move(self, board: Board) -> str:
    if not self._cache:
      self.color = board.current_player_color
      self.minmax(state=GameState.IN_PROGRESS, board=board, depth=0, maximizing_player=True, alpha=-float('inf'), beta=float('inf'))
    return self._cache