# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL

import ctypes
import os
import sys
import locale

def get_system_encoding():
    """Get the appropriate encoding for the Windows system"""
    import subprocess
    try:
        # Get the active code page
        result = subprocess.run('chcp', capture_output=True, shell=True, text=True)
        code_page = result.stdout.strip().split(':')[-1].strip()
        return f'cp{code_page}'
    except:
        # Fallback to default encoding
        return locale.getpreferredencoding()

class EngineDLL:
    def __init__(self):
        self.encoding = get_system_encoding()
        print(f"Using encoding: {self.encoding}")
        path = os.path.join(os.getcwd(), "engine.dll")
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
            encoded_string = game_string.encode(self.encoding)
            return self.dll.startGame(encoded_string)
        except UnicodeEncodeError as e:
            print(f"Encoding error: {e}")
            # Fallback to ASCII if encoding fails
            return self.dll.startGame(game_string.encode('ascii', errors='replace'))

    def play_move(self, move_string):
        """Play a move in the current game"""
        if not isinstance(move_string, str):
            raise TypeError("move_string must be a string")
        encoded_string = move_string.encode(self.encoding)
        return self.dll.playMove(encoded_string)

    def get_valid_moves(self):
        """Get list of valid moves with debug info"""
        print("Calling validMoves()...")
        try:
            result = self.dll.validMoves()
            print(f"Got result pointer: {result}")
            if result:
                raw_bytes = ctypes.string_at(result)
                print(f"Raw bytes length: {len(raw_bytes)}")
                print(f"Raw bytes: {[hex(b) for b in raw_bytes[:20]]}")  # First 20 bytes
                try:
                    decoded = raw_bytes.decode(self.encoding)
                    print(f"Decoded string (first 50 chars): {decoded[:50]}")
                    return decoded
                except UnicodeDecodeError as e:
                    print(f"Decode error: {e}")
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

import engineInterface
# Test script
try:
    # Test with different encodings
    engineInterface.startGame("Base+MLP")
    print("Game started successfully!")
    
    board = engineInterface.getBoard()
    print("Current board:", board)
    
    moves = engineInterface.getValidMoves()
    print("Valid moves:", moves)
    
except Exception as e:
    print(f"Error: {e}")