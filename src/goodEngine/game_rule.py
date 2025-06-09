# compile with  g++ -shared -o engine.dll *.cpp -static -static-libgcc -static-libstdc++ -I. -O2 -Wall -DBUILDING_DLL
# for linux compile with g++ -fPIC -shared -o engine.dll *.cpp




import ctypes
from abc import ABC, abstractmethod
import os
import numpy as np

EBoardP =ctypes.c_void_p
argType=ctypes.c_int
actionT=ctypes.c_uint64


class game_rule(ABC):
    @abstractmethod
    def init_state(self): 
        pass
    def next_state(self, state, action:actionT):
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
        self.updateState_low = dll._Z10next_stateP6EBoardm
        self.getAction_low = dll._Z10getActionsP6EBoardPm
        self.checkGameStatus_low = dll._Z11checkStatusP6EBoard
        self.actionToString_low = dll._Z14actionToStringmP6EBoardPc
        #self.printAct_low = dll._Z16printActionFancym
        self.PrintBoard_low = dll._Z10printBoardP6EBoard  
        self.evalBoard_low = dll._Z9boardEvalP6EBoardPd
        self.delBoardC_low=dll._Z8delBoardP6EBoard
        self.stringToAction_low=dll._Z14stringToActionP6EBoardPc
        #self.boardToCVect_low=dll._Z9BoardRappP6EBoard
        self.getStatusVector_low=dll._Z15getStatusVectorP6EBoard
        self.getAssociatedAction_low=dll._Z19getAssociatedActionP6EBoard
        self.getMask_low=self.dll._Z7getMaskP6EBoard
        # Set argument/return types
        self.getBoard_low.argtypes = [argType]  # the type of game, define in engine/enums.h
        self.getBoard_low.restype = EBoardP

        self.copyBoard.argtypes = [EBoardP]  
        self.copyBoard.restype = EBoardP

        self.updateState_low.argtypes = [EBoardP, actionT]
        self.updateState_low.restype = None

        self.getAction_low.argtypes = [EBoardP, ctypes.POINTER(actionT)]
        self.getAction_low.restype = None

        self.checkGameStatus_low.argtypes = [EBoardP]
        self.checkGameStatus_low.restype = ctypes.c_int

        
        #self.printAct_low.argtypes = [actionT]
        #self.printAct_low.restype = None


        self.evalBoard_low.argtypes = [EBoardP, ctypes.POINTER(ctypes.c_double)]
        self.evalBoard_low.restype = ctypes.c_double 


        self.actionToString_low.argtypes = [actionT,EBoardP,ctypes.POINTER(ctypes.c_char)]
        self.actionToString_low.restype = None

        self.stringToAction_low.argtypes=[EBoardP,ctypes.c_char_p]
        self.stringToAction_low.restype=actionT

        #self.boardToCVect_low.argtypes=[EBoardP]
        #self.boardToCVect_low.restype=ctypes.POINTER(ctypes.c_char)        
        self.actions = (actionT * self.MAX_ACTIONS)()

        self.getMask_low.argtypes = [ctypes.c_void_p]
        self.getMask_low.restype = ctypes.POINTER(ctypes.c_int8)
        
        self.getAssociatedAction_low.argtypes = [ctypes.c_void_p]
        self.getAssociatedAction_low.restype = ctypes.POINTER(actionT)

        self.getStatusVector_low.argtypes = [ctypes.c_void_p]
        self.getStatusVector_low.restype = ctypes.POINTER(actionT)



        self.strBuff=(ctypes.c_char* 30)()
    def init_state(self,gametype=0) -> EBoardP:
        return self.getBoard_low(gametype)  # default game type  0 ->base, ..., 7 -> MPL

    def next_state(self, state:EBoardP, action:actionT):
        self.updateState_low(state, ctypes.c_uint64(action))

    def getActions(self, state:EBoardP)->ctypes.POINTER(actionT) :
        self.getAction_low(state, self.actions)  # TODO: Mettere copia
        return self.actions

    def checkStatus(self, state:EBoardP) :
        code = self.checkGameStatus_low(state)
        status_map = {
            0: "OK",
            1: "Game running",
            2: "GAME_OVER_WHITE_WINS",
            3: "GAME_OVER_BLACK_WINS"
        }
        return code

    def actionToString(self, action:actionT,board:EBoardP):
        self.actionToString_low(action,board,self.strBuff)
        ris=""
        i=0
        while(self.strBuff[i]!=b"\0"):
            ris+=self.strBuff[i].decode()
            i+=1
        return ris


    def print_action(self, action:actionT) -> None:
        print("DOIMPLEMENT")
        return
        self.printAct_low(ctypes.c_uint64(action))

    def PrintState(self, state:EBoardP) -> None:
        self.PrintBoard_low(state)

    def calcVal(self,state:EBoardP,w) -> ctypes.c_double:
        return self.evalBoard_low(state,w)
        
    def delBoard(self,state:EBoardP):
        self.delBoardC_low(state)
    
    
    def toVect(self,state:EBoardP):   # to get the status of the board as a np vector : 1024 + 6 + 2 + 1
        ptr = self.getStatusVector_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1033,))
        return arr.copy().astype(int)


    def stringToAction(self,state:EBoardP,str):
        return self.stringToAction_low(state,str.encode())


    #def getAllActions(self,state):
    #    act=self.getActions(state)
    #    ris=""
    #    for i in range(1,act[0]+1):
    #        ris+=";"+self.actionToString(act[i],state)
    #    return ris
    
    
    def get_mask(self, state):          # mask for actions
        ptr = self.getMask_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1575,))
        return arr.copy().astype(bool)  # copy to own Python memory

    def get_actions(self,state):   # all actions masked by getMask
        ptr = self.getAssociatedAction_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1575,))
        return arr.copy().astype(int)  # copy to own Python memory


