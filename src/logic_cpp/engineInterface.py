import ctypes
import os
import sys

# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL

def load_dll():
    path = os.path.join(os.getcwd(), "engine.dll")
    return ctypes.CDLL(path)

try:
    Cengine = load_dll()
    
    # Configure function signatures with proper string encoding
    sin = Cengine._sin
    sin.argtypes = [ctypes.c_double]
    sin.restype = ctypes.c_double

    start_Game = Cengine.startGame
    start_Game.argtypes = [ctypes.c_char_p]  # Expects bytes, not string
    start_Game.restype = None

    play_Move = Cengine.playMove
    play_Move.argtypes = [ctypes.c_char_p]
    play_Move.restype = ctypes.c_int

    getValid_Moves = Cengine.validMoves
    getValid_Moves.restype = ctypes.c_char_p
    
    get_Board = Cengine.getBoard
    get_Board.restype = ctypes.c_char_p

    undo = Cengine.undo
    undo.argtypes = [ctypes.c_int]
    
    # Wrapper functions to handle string encoding/decoding
    def startGame(game_string):
        """
        Wrapper for startGame that handles string encoding
        """
        if not isinstance(game_string, str):
            raise TypeError("game_string must be a string")
        encoded_string = game_string.encode('utf-8')
        return start_Game(encoded_string)
    
    def playMove(move_string):
        """
        Wrapper for playMove that handles string encoding
        """
        if not isinstance(move_string, str):
            raise TypeError("move_string must be a string")
        encoded_string = move_string.encode('utf-8')
        return play_Move(encoded_string)
    
    def getValidMoves():
        """
        Wrapper for getValidMoves that handles string decoding
        """
        result = getValid_Moves()
        return result.decode('utf-8') if result else ""
    
    def getBoard():
        """
        Wrapper for getBoard that handles string decoding
        """
        result = get_Board()
        return result.decode('utf-8') if result else ""

except Exception as e:
    print(f"Failed to initialize engine interface: {e}")
    raise

try:
    print(startGame("Base+MLP"))
    print("Game started successfully!")
    
    # Test other functions
    moves = getValidMoves()
    print("Valid moves:", moves)
    
    board = getBoard()
    print("Current board:", board)
    
except Exception as e:
    print(f"Error: {e}")