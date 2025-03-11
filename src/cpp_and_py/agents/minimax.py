from BoardModel import BoardModel
from agents.strategy import Strategy
from game_logic.enums import *
from copy import deepcopy
import time

# TODO: make a better order of moves to make alpha-beta better
# TODO: Improve heuristic:
#         how many moves we have (wrt to the adversary)
#         how many pieces we have around my queen (if it's my turn or if it's the other player's)
#         how deep is the tree


MIN_EVAL = -10000
MAX_EVAL =  10000
class Minimax(Strategy):
  """
  AI agent following a custom minimax with alpha-beta pruning policy.
  """
  color = PlayerColor.WHITE

  def utility(self, state, board: BoardModel):
    """
    Heuristic function evaluating the current game state from the AI's perspective.

    The heuristic value is defined as follows:

    - If the game is a draw, the value is 0.
    - If the AI wins, the value is 100.
    - If the opponent wins, the value is -100.
    - If the AI is white, the value is the difference between the number of bugs near the AI's queen and the number of bugs near the opponent's queen.
    - If the AI is black, the value is the difference between the number of bugs near the AI's queen and the number of bugs near the opponent's queen.

    This heuristic is very simple and does not take into account other factors such as the number of moves left, the number of pieces on the board, etc.

    :param state: The current game state.
    :type state: GameState
    :param board: The current game board.
    :type board: Board
    :return: The heuristic value of the current game state.
    :rtype: int
    """
    if state == GameState.DRAW:
        return 0
    if self.color == PlayerColor.WHITE:
      if state == GameState.WHITE_WINS:
        return 100
      if state == GameState.BLACK_WINS:
        return -100
      
      return board.get_oracle_eval()
    else:
      if state == GameState.BLACK_WINS:
        return 100
      elif state == GameState.WHITE_WINS:
        return -100
      return board.get_oracle_eval()

  def initiate_minimax(self, board: BoardModel):
    """
    Starts the minimax tree by checking all possible moves and returning the one with the highest utility.

    If no actions are available, it returns the PASS command.

    :param board: The current game board.
    :type board: Board
    :return: The best move according to the minimax algorithm.
    :rtype: str
    """
    start = time.perf_counter()
    max_eval = MIN_EVAL
    alpha = MIN_EVAL
    todo_action = Command.PASS
    # For every action available, play it and calculate the utility (recursively)
    for action in board.valid_moves.split(";"):
      bb = deepcopy(board)
      bb.play(action)
      eval = self.minmax(state=bb.state, board=board, depth=1, maximizing_player=False, alpha=alpha, beta=MAX_EVAL)
      if (eval > max_eval):
        max_eval = eval
        todo_action = action
      alpha = max(alpha, max_eval)
    end = time.perf_counter()
    print(end-start)
    return todo_action
  
  def heuristic_eval_board(self, state, board, color):
     return 0

  def minmax(self, state, board, depth, maximizing_player, alpha, beta):
    """
    Performs the minimax algorithm with alpha-beta pruning to evaluate and return the utility of a game state.

    The function recursively explores all possible moves up to a specified depth to determine the best move for the
    current player. It optimizes the search process by pruning branches that cannot produce a better outcome than
    previously examined branches.

    :param state: The current game state.
    :type state: GameState
    :param board: The current game board.
    :type board: Board
    :param depth: The current depth in the game tree.
    :type depth: int
    :param maximizing_player: Indicates whether the current player is maximizing or minimizing their score.
    :type maximizing_player: bool
    :param alpha: The best already explored option along the path to the root for the maximizer.
    :type alpha: float
    :param beta: The best already explored option along the path to the root for the minimizer.
    :type beta: float
    :return: The utility value of the current game state.
    :rtype: int
    """
    if state == GameState.DRAW or state == GameState.WHITE_WINS or state == GameState.BLACK_WINS or depth >= self.depth_limit:
        return self.heuristic_eval_board(state, board,self.color)
    
    if maximizing_player:
        max_eval = MIN_EVAL
        for action in board.valid_moves.split(";"):
            bb = deepcopy(board)
            bb.play(action)
            eval = self.minmax(bb.state, bb, depth + 1, False, alpha, beta)
            max_eval = max(max_eval, eval)
            alpha = max(alpha, max_eval)
            # We cut all the remaining children
            if beta <= alpha:
                break  
        return max_eval
    else:
        min_eval = MAX_EVAL
        for action in board.valid_moves.split(";"):
            bb = deepcopy(board)
            bb.play(action)
            eval = self.minmax(bb.state, bb, depth + 1, True, alpha, beta)
            min_eval = min(min_eval, eval)
            beta = min(beta, min_eval)
            # We cut all the remaining children
            if beta <= alpha:
                break  
        return min_eval

  def calculate_best_move(self, board: BoardModel) -> str:
    """
    Calculates the best move for the current player given the state of the board.

    If this is the first time the method is called, it will start the minimax tree and calculate the best move.
    Otherwise, it simply returns the cached result.

    :param board: The current game board.
    :type board: Board
    :return: The best move according to the minimax algorithm.
    :rtype: str
    """
    
    if not self._cache or self.DISABLE_CACHE:
      self.color = board.current_player_color()
      self._cache = self.initiate_minimax(board)
    return self._cache