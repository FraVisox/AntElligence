# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL
# for linux compile with g++ -fPIC -shared -o engine.dll *.cpp

import ctypes
import os
from BoardModel import BoardModel

# This represents the return types of the functions
class ReturnTypes:
    OK = 0
    ERROR = 1
    GAME_OVER_DRAW = 2
    GAME_OVER_WHITE_WINS = 3
    GAME_OVER_BLACK_WINS = 4
    INVALID_ARGUMENT = 5
    INVALID_GAME_NOT_STARTED = 6

class EngineDLL(BoardModel):
    def __init__(self,gamestring: str = ""):
        path = os.path.join(os.getcwd(), "engine_cpp/engine.dll")
        self.dll = ctypes.CDLL(path)
        self._setup_functions()
        self.start_game(gamestring)


    def _setup_functions(self):
        # Set up game functions
        self.dll.startGame.argtypes = [ctypes.c_char_p]
        self.dll.startGame.restype = ctypes.c_int

        self.dll.playMove.argtypes = [ctypes.c_char_p]
        self.dll.playMove.restype = ctypes.c_int

        self.dll.validMoves.restype = ctypes.c_char_p

        self.dll.getBoard.restype = ctypes.c_char_p
        
        self.dll.undo.argtypes = [ctypes.c_int]
        
        self.dll.getTurn.restype = ctypes.c_int

        self.dll.oracleEval.restype=ctypes.c_float

    def current_player_turn(self)->int:
        return self.dll.getTurn()

    def current_player_queen_in_play(self)->bool:
        raise "Not implemented"
        return false
    def valid_moves(self)->str:
        return self.get_valid_moves()

    def play(self,move_string: str)-> None:
        self.play_move(move_string)

    def start_game(self, game_string):
        """Start a new game with the given configuration string"""
        encoded_string = game_string.encode("utf-8")
        return self.dll.startGame(encoded_string) == ReturnTypes.OK

    def play_move(self, move_string):
        """Play a move in the current game"""
        encoded_string = move_string.encode("utf-8")
        return self.dll.playMove(encoded_string)

    def get_valid_moves(self):
        """Get list of valid moves with debug info"""
        try:
            result = self.dll.validMoves()
            if result:
                raw_bytes = ctypes.string_at(result)
                try:
                    decoded = raw_bytes.decode("utf-8")
                    return decoded
                except UnicodeDecodeError as e:
                    return raw_bytes.decode('ascii', errors='replace')
        except Exception:
            return ""

    def get_board(self):
        """Get current board state"""
        result = self.dll.getBoard()
        if result:
            try:
                return ctypes.string_at(result).decode("utf-8")
            except UnicodeDecodeError:
                return ctypes.string_at(result).decode('ascii', errors='replace')
        return ""

    def undo(self, amount):
        """Undo specified number of moves"""
        return self.dll.undo(amount)
    
    def get_oracle_eval(self) -> float:
        return self.dll.oracleEval()
# Create a singleton instance
engine = EngineDLL()

# Export functions at module level for backwards compatibility
#startGame = engine.start_game
#playMove = engine.play_move
#getValidMoves = engine.get_valid_moves
#getBoard = engine.get_board
#undo = engine.undo