
#https://docs.python.org/3.5/library/ctypes.html#fundamental-data-types
# to compile: g++ -fPIC -shared -o engine.so engine.cpp

# or use https://github.com/pybind/pybind11

import ctypes
import pathlib
path=pathlib.Path().absolute().joinpath("engine.so")
Cengine=ctypes.CDLL(path)

Cengine.add.argtypes = (ctypes.c_int, ctypes.c_int)
Cengine.add.restype = ctypes.c_int


# how to call a function: Cengine._[outputType]functionName[inputType](params)


# TODO: make it give exceptions if the board is not initialized o qualcosa così
def startGame(s):
    Cengine._Z9startGamePc(s)

# TODO: manda eccezione se non e' inizializzato
def playMove(s):
    Cengine.Z8playMovePc(s)

def getValidMoves():  # TODO:  learn how to return a string that represents all possible moves
    return Cengine._Z8allMovesv()

# TODO: da implementare: deve ritornare la stringa della board
def getBoard():
    return Cengine._Z8getBoardv()

# TODO: manda eccezione se non e' inizializzato
def undo(amount):
    Cengine._Z4undov(amount) # TODO: pass the number of moves to undo to the function