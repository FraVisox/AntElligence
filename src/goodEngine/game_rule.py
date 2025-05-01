# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL
# for linux compile with g++ -fPIC -shared -o engine.dll *.cpp

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
        self.next_stateS=self.dll._Z10next_statePcS_S_
        self.getActionsS=self.dll._Z10getActionsPc
        self.checkStatusS=self.dll._Z11checkStatusPc

        
        self.next_stateS.argtypes = [ctypes.c_int8*282,ctypes.c_int8*282,ctypes.c_int8*9]
        self.next_stateS.restype = ctypes.c_char*282
        
        self.getActionsS.argtypes = [ctypes.ctypes.c_int_p;ctypes.c_int8*282]
        self.getActionsS.argtypes
        
        self.checkStatusS.argtypes = [ctypes.c_int8*282]
        self.checkStatusS.restype = ctypes.c_char
        
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

