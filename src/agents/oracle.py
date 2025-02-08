from old_engine_py.game_logic.board import Board
from old_engine_py.game_logic.board import Bug
from old_engine_py.game_logic.enums import *
from copy import deepcopy


# TODO: Improve heuristic:
#         how many moves we have (wrt to the adversary)
#         how many pieces we have around my queen (if it's my turn or if it's the other player's)
#         how deep is the tree
class Oracle:
	# HEURISTIC
	@staticmethod
	def heuristic_eval_board(state, board : Board,player_color):
			"""
			Heuristic function evaluating the current game state from the AI's perspective. The value is from -100 (game lost) to +100 (game win)

			The heuristic value is defined as follows:

			- If the game is a draw, the value is 0.
			- If the AI wins, the value is 100.
			- If the opponent wins, the value is -100.
			- If the AI is white, the value is the difference between the number of bugs near the AI's queen and the number of bugs near the opponent's queen.
			- If the AI is black, the value is the difference between the number of bugs near the AI's queen and the number of bugs near the opponent's queen.

			:param state: The current game state.
			:type state: GameState
			:param board: The current game board.
			:type board: Board
			:return: The heuristic value of the current game state.
			:rtype: int
			"""
			if state == GameState.DRAW:
					return 0
			enemy_color = PlayerColor.WHITE if player_color==PlayerColor.BLACK else PlayerColor.BLACK

			if state == GameState.WHITE_WINS:
				return 100 if player_color==PlayerColor.WHITE else -100
			if state == GameState.BLACK_WINS:
				return 100 if player_color==PlayerColor.BLACK else -100
			if state == GameState.DRAW:
				return 0
			
			C1a =0# board._get_number_of_bugs_near_queen(player_color) # minimize
			C1b = board._get_number_of_bugs_near_queen(enemy_color)  # maximize

			
			#C2a = sum(Oracle.bugMoveVal[mov.bug.type] for mov in board.moves if mov.bug.color==player_color) #maximize
			#C2b = sum(Oracle.bugMoveVal[mov.bug.type] for mov in board.moves if mov.bug.color==enemy_color)  # minimize
			return (C1b)#-(C2a-C2b)

	bugMoveVal={
		BugType.BEETLE:1,
		BugType.GRASSHOPPER:1,
		BugType.LADYBUG: 1,
		BugType.MOSQUITO : 1,
		BugType.PILLBUG : 1,
		BugType.SOLDIER_ANT:2,
		BugType.SPIDER:1,
		BugType.QUEEN_BEE:4
		
	}