from numpy.ctypeslib import ctypes as ctypes
from abc import ABC, abstractmethod
import os

import copy
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.multiprocessing as mp
from tqdm.notebook import trange
#torch.use_deterministic_algorithms(True)

import random
import math

#torch.manual_seed(0)
#random.seed(0)
#np.random.seed(0)

EBoardP =ctypes.c_void_p
MCTSP =ctypes.c_void_p
argType=ctypes.c_int
actionT=ctypes.c_uint16
pointer_action=ctypes.POINTER(actionT)

# FIXME: quando vedi i figli salvali


# Abstract class that implements the game rules
class game_rule(ABC):
    @abstractmethod
    def init_state(self):
        pass
    def next_state(self, state, action:actionT):
        pass
    def checkStatus(self, state):
        pass

# Our game's rules
class DLLGameRule(game_rule):

    
    def __init__(self):
        global MAX_ACTIONS
        path = os.path.join(os.getcwd(), '/home/matteo/Desktop/Elicsir/AntElligence/src/goodEngine/cpp/build/interface_reduced.so')
        dll = ctypes.CDLL(path)
        self.dll=dll

        # board management basic function
        self.getBoard_low = dll._Z10base_statei
        self.copyBoard_low = dll._Z9copyBoardP6EBoard
        self.updateState_low = dll._Z10next_stateP6EBoardt
        self.checkGameStatus_low = dll._Z11checkStatusP6EBoard
        self.PrintBoard_low = dll._Z10printBoardP6EBoard
        self.delBoardC_low=dll._Z8delBoardP6EBoard
        self.stringToAction_low=dll._Z14stringToActionP6EBoardPc
        self.actionToString_low=dll._Z14actionToStringtP6EBoardPc
        
        self.getAssociatedAction_low=dll._Z19getAssociatedActionP6EBoard
        self.getMask_low=self.dll._Z7getMaskP6EBoard
        self.compute_possible_low=self.dll._Z19updatePossiblemovesP6EBoard;
        self.getActions_low=dll._Z10getActionsP6EBoardPt
        self.getValue_low=dll._Z9boardEvalP6EBoard;
        #self.getStatusVector_low=dll._Z15getStatusVectorP6EBoard
        #self.create_MCTS_low=self.dll._Z10createMCTSPc
        #self.search_MCTS_low=self.dll._Z10searchMCTSP4MCTSP6EBoardi
        #self.delete_MCTS_low=self.dll._Z10deleteMCTSP4MCTS
        
        # Set argument/return types
        self.getBoard_low.argtypes = [argType]  # the type of game, define in engine/enums.h
        self.getBoard_low.restype = EBoardP

        self.copyBoard_low.argtypes = [EBoardP]
        self.copyBoard_low.restype = EBoardP

        self.updateState_low.argtypes = [EBoardP, actionT]
        self.updateState_low.restype = None

        self.checkGameStatus_low.argtypes = [EBoardP]
        self.checkGameStatus_low.restype = ctypes.c_int

        self.stringToAction_low.argtypes=[EBoardP,ctypes.c_char_p]
        self.stringToAction_low.restype=actionT

        self.actionToString_low.argtypes=[actionT,EBoardP,ctypes.POINTER(ctypes.c_char)]
        self.actionToString_low.restype=None
        
        self.getMask_low.argtypes = [ctypes.c_void_p]
        self.getMask_low.restype = ctypes.POINTER(ctypes.c_uint8)

        self.getAssociatedAction_low.argtypes = [ctypes.c_void_p]
        self.getAssociatedAction_low.restype = ctypes.POINTER(actionT)

        self.getActions_low.argtypes=[EBoardP, ctypes.POINTER(actionT)]
        self.getActions_low.restype=None

        self.getValue_low.argtypes=[EBoardP]
        self.getValue_low.restype=ctypes.c_double
        
        """
        self.getStatusVector_low.argtypes = [ctypes.c_void_p]
        self.getStatusVector_low.restype = ctypes.POINTER(actionT)

        self.create_MCTS_low.argtypes=[ctypes.c_void_p]
        self.create_MCTS_low.restype=MCTSP
        
        self.search_MCTS_low.argtypes=[MCTSP,EBoardP,ctypes.c_int]
        self.search_MCTS_low.restype=ctypes.POINTER(ctypes.c_double)
        
        self.delete_MCTS_low.argtypes=[MCTSP]
        self.delete_MCTS_low.restype=None
        """
        # maybe usefull buffers
        self.actions = (actionT * MAX_ACTIONS)()
        self.strBuff=(ctypes.c_char* 30)()


    
    # Returns the initial state
    def init_state(self,gametype=7) -> EBoardP:
        return self.getBoard_low(gametype)  # default game type  0 ->base, ..., 7 -> MPL


    # Update state given an action. action is a number. Doesn't return anything
    def next_state(self, state:EBoardP, action:actionT): # FIXME: fa l'update della board che ci passi (action come intero)
        self.updateState_low(state, actionT(action))

    def copyBoard(self,state):
        CopyAddr=self.copyBoard_low(state)
        return CopyAddr
    # Get the set of possible actions. It is a vector of actions (ints).
    STATUS_OK = 0
    STATUS_INVALID = 1
    STATUS_WHITE_WINS = 2
    STATUS_BLACK_WINS = 3

    # Check if someone has won
    def checkStatus(self, state:EBoardP): # FIXME: ritorna il numero in base a cosa è successo
        code = self.checkGameStatus_low(state)
        status_map = {
            0: "OK",
            1: "INVALID_GAME_NOT_STARTED",
            2: "GAME_OVER_WHITE_WINS or DRAW",
            3: "GAME_OVER_BLACK_WINS",
            4: "GAME_OVER_DRAW"
        }
        return code



    # Prints the board
    def PrintState(self, state:EBoardP): # FIXME: for debug
        self.PrintBoard_low(state)

    # Deletes the board
    def delBoard(self,state:EBoardP):
        self.delBoardC_low(state)

    # Converts a string to an action
    def stringToAction(self,state:EBoardP,str): #FIXME: ritorna un intero
        return self.stringToAction_low(state,str.encode())

    def actionToString(self,action,state):
        self.actionToString_low(action,state,self.strBuff)
        i=0
        while self.strBuff[i]!=b'\x00':
            i+=1
        return self.strBuff[0:i].decode()
        
    def get_mask(self, state):          # mask for actions
        ptr = self.getMask_low(state)
        return np.ctypeslib.as_array(ptr, shape=(MAX_ACTIONS,)).astype(bool)

    def get_actions(self,state):   # all actions masked by getMask
        ptr = self.getAssociatedAction_low(state)
        return np.ctypeslib.as_array(ptr, shape=(MAX_ACTIONS,)).astype(np.uint64)

    def compute_actions(self,state:EBoardP):
        self.compute_possible_low(state)
    def getActions(self,state):
        self.getActions_low(state,self.actions)
        return self.actions
    def calcVal(self,state:EBoardP):
        return self.getValue_low(state)
    # MCTS fucntions - deleted in this version
    """
    # Converts the board to a vector of chars (284)
    def toVect(self,state:EBoardP): # FIXME: sarebbe ottimo ritornare un numpy vector di 32*32 CNN 32*32 più altri neuroni (6 in teoria, +1 che rappresenta il turno (è endgame o startgame) +2 che sono i bug mossi appena adesso) che ci dicono chi è sopra a chi e cose del genere
        ptr = self.getStatusVector_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1033,))
        return torch.from_numpy(arr.astype(np.float32))
    "
    def toVect_split(self, state: EBoardP) -> tuple[torch.FloatTensor, torch.FloatTensor]:
        ptr = self.getStatusVector_low(state)

        arr = np.ctypeslib.as_array(ptr, shape=(1033,)).astype(np.float32)
        t = torch.from_numpy(arr).to(device, non_blocking=True)

        board    = t[:1024].view(1, 32, 32)
        metadata = t[1024:]

        return board, metadata
        #arr = np.ctypeslib.as_array(ptr, shape=(1033,)).astype(np.float32)

        #board = torch.from_numpy(arr[:1024]).view(1, 32, 32).to(device)
        #metadata = torch.from_numpy(arr[1024:]).to(device)
        #return board, metadata

    def get_opponent_value(self, value):
        return -value

    def get_opponent(self, player): # TODO: il player è 1 o -1
        return -player
    
    def create_mcts(self,model_path: str) -> MCTSP:
        return self.create_MCTS_low(model_path.encode('utf-8'))

    def search_mcts(self,tree: MCTSP, board: EBoardP, turn: int = 1) -> np.ndarray:
        global MAX_ACTIONS
        raw_ptr = self.search_MCTS_low(tree, board, turn)
        if not raw_ptr:
            raise RuntimeError("searchMCTS returned NULL")
        arr = np.ctypeslib.as_array(raw_ptr, shape=(MAX_ACTIONS,)).astype(np.double)
        print(arr,MAX_ACTIONS)
        return arr
    
    def delete_mcts(self,tree: MCTSP) -> None:
        self.delete_MCTS_low(tree)
    """
BOARD_VECTOR_SIZE = 32*32
MAX_ACTIONS=1575
game=DLLGameRule()
