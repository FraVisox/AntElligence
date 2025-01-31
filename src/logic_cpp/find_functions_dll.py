import ctypes
import os
import sys

def print_dll_info(dll):
    print("\nDLL Info:")
    print("Loaded at:", dll._handle)
    print("Available attributes:", dir(dll))
    try:
        # Try to get the address of each function
        print("\nFunction addresses:")
        print("startGame:", dll.startGame._address)
    except Exception as e:
        print(f"Error getting function addresses: {e}")

path = os.path.join(os.getcwd(), "engine.dll")
print(f"Loading DLL from: {path}")

try:
    Cengine = ctypes.CDLL(path)
    print_dll_info(Cengine)
    
    # Configure function signatures
    
    startGame = Cengine.startGame
    startGame.argtypes = [ctypes.c_char_p]
    startGame.restype = ctypes.c_int
    
    # Test startGame function
    print("\nTesting startGame function:")
    test_str = b"Base+MLP"  # Note: using bytes instead of string
    print(startGame(test_str))
    print("startGame called successfully")
    
except AttributeError as e:
    print(f"\nFunction not found error: {e}")
    print("This usually means the function wasn't properly exported from the DLL")
except Exception as e:
    print(f"\nOther error: {e}")