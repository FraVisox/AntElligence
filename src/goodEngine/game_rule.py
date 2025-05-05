# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL
# for linux compile with g++ -fPIC -shared -o engine.dll *.cpp




import ctypes
from abc import ABC, abstractmethod
import os

class game_rule(ABC):
    @abstractmethod
    def init_state(self):
        pass
    def next_state(self, state, action):
        pass    
    def getActions(self, state):
        pass
    def checkStatus(self, state):
        pass

class DLLGameRule(game_rule):

    MAX_ACTIONS = 256
    def __init__(self):
        path = os.path.join(os.getcwd(), "cpp/interface.so")
        dll = ctypes.CDLL(path)
        self.dll=dll
        self.getBoard_low = dll._Z10base_statei
        self.copyBoard = dll._Z9copyBoardP6EBoard
        self.updateState_low = dll._Z10next_stateP6EBoardl
        self.getAction_low = dll._Z10getActionsP6EBoardPl
        self.checkGameStatus_low = dll._Z11checkStatusP6EBoard
        self.actionToString_low = dll._Z14actionToStringl
        self.printAct_low = dll._Z16printActionFancyl
        self.PrintBoard_low = dll._Z10PrintBoardP6EBoard  
        self.evalBoard_low = dll._Z9boardEvalP6EBoardPd
        self.delBoardC_low=dll._Z8delBoardP6EBoard
        self.stringToAction_low=dll._Z14stringToActionP6EBoardPc
        self.boardToCVect_low=dll._Z9BoardRappP6EBoard
        # Set argument/return types
        self.getBoard_low.argtypes = [ctypes.c_int]  # the type of game, define in engine/enums.h
        self.getBoard_low.restype = ctypes.c_void_p

        self.copyBoard.argtypes = [ctypes.c_void_p]  
        self.copyBoard.restype = ctypes.c_void_p

        self.updateState_low.argtypes = [ctypes.c_void_p, ctypes.c_int64]
        self.updateState_low.restype = None

        self.getAction_low.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int64)]
        self.getAction_low.restype = None

        self.checkGameStatus_low.argtypes = [ctypes.c_void_p]
        self.checkGameStatus_low.restype = ctypes.c_int

        self.actionToString_low.argtypes = [ctypes.c_int64]
        self.actionToString_low.restype = ctypes.c_char_p

        self.printAct_low.argtypes = [ctypes.c_int64]
        self.printAct_low.restype = None


        self.evalBoard_low.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_double)]
        self.evalBoard_low.restype = ctypes.c_double 



        self.stringToAction_low.argtypes=[ctypes.c_void_p,ctypes.c_char_p]
        self.stringToAction_low.restype=ctypes.c_int64

        self.boardToCVect_low.argtypes=[ctypes.c_void_p]
        self.boardToCVect_low.restype=ctypes.POINTER(ctypes.c_char)        
        self.actions = (ctypes.c_int64 * self.MAX_ACTIONS)()

    def init_state(self,gametype=0):
        return self.getBoard_low(gametype)  # default game type

    def next_state(self, state, action):
        self.updateState_low(state, ctypes.c_int64(action))

    def getActions(self, state):
        self.getAction_low(state, self.actions)
        return self.actions

    def checkStatus(self, state):
        code = self.checkGameStatus_low(state)
        status_map = {
            0: "OK",
            1: "INVALID_GAME_NOT_STARTED",
            2: "GAME_OVER_WHITE_WINS or DRAW",
            3: "GAME_OVER_BLACK_WINS"
        }
        return code

    def actionToString(self, action):
        return self.actionToString_low(ctypes.c_int64(action)).decode('utf-8')

    def print_action(self, action):
        self.printAct_low(ctypes.c_int64(action))

    def PrintState(self, state):
        self.PrintBoard_low(state)

    def calcVal(self,state,w):
        return self.evalBoard_low(state,w)
    def delBoard(self,state):
        self.delBoardC_low(state)
    def toVect(self,state):
        vc=self.boardToCVect_low(state)
        return [vc[i] for i in range(284)]
    def stringToAction(self,state,str):
        return self.stringToAction_low(state,str.encode())


    def getAllActions(self,state):
        act=self.getActions(state)
        return ";".join([self.actionToString_low(act[i]).decode() for i in range(1,act[0]+1)])

        