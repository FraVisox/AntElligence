
#https://docs.python.org/3.5/library/ctypes.html#fundamental-data-types
# to compile: g++ -fPIC -shared -o engine.so engine.cpp

# or use https://github.com/pybind/pybind11

import ctypes
import pathlib
path=pathlib.Path().absolute().joinpath("engine.so")
Cengine=ctypes.CDLL(path)

# how to call a function: Cengine._[outputType]functionName[inputType](params)


# TODO: make it give exceptions if the board is not initialized o qualcosa così
startGame = Cengine.startGame
startGame.argtypes = (ctypes.c_wchar_p)

# TODO: manda eccezione se non e' inizializzato
playMove = Cengine.playMove
playMove.argtypes = (ctypes.c_wchar_p)

# TODO: vedi se funzia
getValidMoves = Cengine.allMoves
getValidMoves.restype = (ctypes.c_wchar_p)

# TODO: da implementare: deve ritornare la stringa della board
getBoard = Cengine.getBoard
getBoard.restype = (ctypes.c_wchar_p)

# TODO: manda eccezione se non e' inizializzato
undo = Cengine.undo
undo.argtypes = (ctypes.c_int)