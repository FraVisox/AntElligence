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
    def __init__(self):
        path = os.path.join(os.getcwd(), "cpp/interface.so")
        dll = ctypes.CDLL(path)
        self.dll=dll
        self.getBoard = dll._Z10base_statei
        self.copyBoard = dll._Z9copyBoardP6EBoard
        self.updateState = dll._Z10next_stateP6EBoardl
        self.getAction = dll._Z10getActionsP6EBoardPl
        self.checkGameStatus = dll._Z11checkStatusP6EBoard
        self.actionToString = dll._Z14actionToStringl
        self.printAct = dll._Z16printActionFancyl

        # Set argument/return types
        self.getBoard.argtypes = [ctypes.c_int]  # the type of game, define in engine/enums.h
        self.getBoard.restype = ctypes.c_void_p

        self.copyBoard.argtypes = [ctypes.c_void_p]  
        self.copyBoard.restype = ctypes.c_void_p

        self.updateState.argtypes = [ctypes.c_void_p, ctypes.c_int64]
        self.updateState.restype = None

        self.getAction.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int64)]
        self.getAction.restype = None

        self.checkGameStatus.argtypes = [ctypes.c_void_p]
        self.checkGameStatus.restype = ctypes.c_int

        self.actionToString.argtypes = [ctypes.c_int64]
        self.actionToString.restype = ctypes.c_char_p

        self.printAct.argtypes = [ctypes.c_int64]
        self.printAct.restype = None

        self.PrintBoard = dll._Z10PrintBoardP6EBoard  
        self.PrintBoard.argtypes = [ctypes.c_void_p]  
        self.PrintBoard.restype = None 

    def init_state(self):
        return self.getBoard(0)  # default game type

    def next_state(self, state, action):
        new_state = self.copyBoard(state)
        self.updateState(new_state, ctypes.c_int64(action))
        return new_state

    def getActions(self, state):
        MAX_ACTIONS = 256
        actions = (ctypes.c_int64 * MAX_ACTIONS)()
        self.getAction(state, actions)
        return [actions[i+1] for i in range(actions[0])]

    def checkStatus(self, state):
        code = self.checkGameStatus(state)
        status_map = {
            0: "OK",
            1: "INVALID_GAME_NOT_STARTED",
            2: "GAME_OVER_WHITE_WINS or DRAW",
            3: "GAME_OVER_BLACK_WINS"
        }
        return status_map.get(code, f"UNKNOWN({code})")

    def action_to_str(self, action):
        return self.actionToString(ctypes.c_int64(action)).decode('utf-8')

    def print_action(self, action):
        self.printAct(ctypes.c_int64(action))

    def PrintState(self, state):
        self.PrintBoard(state)

"""
import ctypes
import os
from abc import ABC, abstractmethod
# This represents the return types of the functions

class game_rule(ABC):
    @abstractmethod
    def init_state():
        pass
    def next_state(self,state,action):
        pass    
    def getActions(self,state):
        pass
    def checkStatus(self,state):
        pass

class game_rule_cpp_imp(game_rule):
    def __init__(self):
        path = os.path.join(os.getcwd(), "cpp/engine.dll")
        self.dll = ctypes.CDLL(path)
        self._setup_functions()


    def _setup_functions(self):
        # Set up game functions
        
        self.getBoard=dll._Z10base_statei
        self.copyBoard=dll._Z9copyBoardP6EBoard
        self.updateState=dll._Z10next_stateP6EBoardl
        self.getAction=dll._Z10getActionsP6EBoardPl
        self.actionToString=dll._Z14actionToStringl
        self.printAct=dll._Z16printActionFancyl

        self.actionToString.restype = ctypes.c_char_p
        self.actionToString.argtypes = [ctypes.c_int64]
        self.printAct.argtypes = [ctypes.c_int64]
        self.getAction.argtypes = [ctypes.c_void_p,ctypes.POINTER(ctypes.c_int64)]
        
    def init_state(self):
        return (ctypes.c_int8*282)()
    
    def next_state(self,state,action):
        v=(ctypes.c_int8*282)()
        self.next_stateS(v,state,action)
        return v
    
    def getActions(self,state):
        act=(ctypes.c_int8*282)()
        self.getActionsS(act,state)
        return act
    
    def checkStatus(self,state):
        return self.checkStatusS(state)
# Create a singleton instance
#engine = EngineDLL()

# Export functions at module level for backwards compatibility
#startGame = engine.start_game
#playMove = engine.play_move
#getValidMoves = engine.get_valid_moves
#getBoard = engine.get_board
#undo = engine.undo
"""
