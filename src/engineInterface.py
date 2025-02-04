# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL

import ctypes
import os
import sys
import locale

# This represents the return types of the functions
class ReturnTypes:
    OK = 0
    INVALID_MOVE = 1
    GAME_OVER = 2
    DRAW = 3
    WHITE_WINS = 4
    BLACK_WINS = 5

class EngineDLL:
    def __init__(self):
        path = os.path.join(os.getcwd(), "engine_cpp/engine.dll")
        self.dll = ctypes.CDLL(path)
        self._setup_functions()

    def _setup_functions(self):
        # Set up game functions
        self.dll.startGame.argtypes = [ctypes.c_char_p]
        self.dll.startGame.restype = ctypes.c_int

        self.dll.playMove.argtypes = [ctypes.c_char_p]
        self.dll.playMove.restype = ctypes.c_int

        self.dll.validMoves.restype = ctypes.c_char_p

        self.dll.getBoard.restype = ctypes.c_char_p
        
        self.dll.undo.argtypes = [ctypes.c_int]

    def start_game(self, game_string):
        """Start a new game with the given configuration string"""
        if not isinstance(game_string, str):
            raise TypeError("game_string must be a string")
        try:
            encoded_string = game_string.encode("utf-8")
            if self.dll.startGame(encoded_string) == 1:
                return False
        except UnicodeEncodeError as e:
            print(f"Encoding error: {e}")
            # Fallback to ASCII if encoding fails
            return self.dll.startGame(game_string.encode('ascii', errors='replace'))

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
        except Exception as e:
            print(f"Error in get_valid_moves: {e}")
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

# Create a singleton instance
engine = EngineDLL()

# Export functions at module level for backwards compatibility
startGame = engine.start_game
playMove = engine.play_move
getValidMoves = engine.get_valid_moves
getBoard = engine.get_board
undo = engine.undo