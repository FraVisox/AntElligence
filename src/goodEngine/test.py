from subprocess import Popen, PIPE
file_exe="../../../Mzinga.LinuxX64/MzingaEngine"
player = Popen(file_exe, stdin=PIPE, stdout=PIPE, stderr=PIPE, encoding="UTF8", bufsize=1)

player.stdout.readline()
player.stdout.readline()
player.stdout.readline()


import time
import random
import ctypes
import math
import time

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from tqdm.notebook import trange

import random
import math
from abc import ABC, abstractmethod
import os
import numpy as np
from numpy.ctypeslib import ctypes as ctypes
from numpy.ctypeslib import ctypes as ctypes
from abc import ABC, abstractmethod
import os
import numpy as np

EBoardP =ctypes.c_void_p
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

    MAX_ACTIONS = 1575
    BOARD_VECTOR_SIZE = 32*32

    def __init__(self):
        path = os.path.join(os.getcwd(), 'cpp/interface.so')
        dll = ctypes.CDLL(path)
        self.dll=dll
        self.getBoard_low = dll._Z10base_statei
        self.copyBoard = dll._Z9copyBoardP6EBoard
        self.updateState_low = dll._Z10next_stateP6EBoardt
        self.checkGameStatus_low = dll._Z11checkStatusP6EBoard
        #self.printAct_low = dll._Z16printActionFancym
        self.PrintBoard_low = dll._Z10printBoardP6EBoard
        self.delBoardC_low=dll._Z8delBoardP6EBoard
        self.stringToAction_low=dll._Z14stringToActionP6EBoardPc
        #self.boardToCVect_low=dll._Z9BoardRappP6EBoard
        self.getStatusVector_low=dll._Z15getStatusVectorP6EBoard
        self.getAssociatedAction_low=dll._Z19getAssociatedActionP6EBoard
        self.getMask_low=self.dll._Z7getMaskP6EBoard
        self.compute_possible_low=self.dll._Z19updatePossiblemovesP6EBoard;

        self.actionToStringLow=self.dll._Z14actionToStringtP6EBoardPc
        
        # Set argument/return types
        self.getBoard_low.argtypes = [argType]  # the type of game, define in engine/enums.h
        self.getBoard_low.restype = EBoardP

        self.copyBoard.argtypes = [EBoardP]
        self.copyBoard.restype = EBoardP

        self.updateState_low.argtypes = [EBoardP, actionT]
        self.updateState_low.restype = None

        self.checkGameStatus_low.argtypes = [EBoardP]
        self.checkGameStatus_low.restype = ctypes.c_int

        self.stringToAction_low.argtypes=[EBoardP,ctypes.c_char_p]
        self.stringToAction_low.restype=actionT

        self.actions = (actionT * self.MAX_ACTIONS)()

        self.getMask_low.argtypes = [ctypes.c_void_p]
        self.getMask_low.restype = ctypes.POINTER(ctypes.c_uint8)

        self.getAssociatedAction_low.argtypes = [ctypes.c_void_p]
        self.getAssociatedAction_low.restype = ctypes.POINTER(actionT)

        self.getStatusVector_low.argtypes = [ctypes.c_void_p]
        self.getStatusVector_low.restype = ctypes.POINTER(actionT)

        self.actionToStringLow.argtypes=[actionT,EBoardP,ctypes.c_char_p]
        self.actionToStringLow.restype=None
        
        self.strBuff=(ctypes.c_char* 30)()

        

    # Returns the initial state
    def init_state(self,gametype=7) -> EBoardP:
        return self.getBoard_low(gametype)  # default game type  0 ->base, ..., 7 -> MPL


    # Update state given an action. action is a number. Doesn't return anything
    def next_state(self, state:EBoardP, action:actionT): # FIXME: fa l'update della board che ci passi (action come intero)
        self.updateState_low(state, actionT(action))

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

    # Converts the board to a vector of chars (284)
    def toVect(self,state:EBoardP): # FIXME: sarebbe ottimo ritornare un numpy vector di 32*32 CNN 32*32 più altri neuroni (6 in teoria, +1 che rappresenta il turno (è endgame o startgame) +2 che sono i bug mossi appena adesso) che ci dicono chi è sopra a chi e cose del genere
        ptr = self.getStatusVector_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1033,))
        return torch.from_numpy(arr.astype(np.float32))

    def toVect_split(self, state: EBoardP) -> tuple[torch.FloatTensor, torch.FloatTensor]:
        ptr = self.getStatusVector_low(state)
        arr = np.ctypeslib.as_array(ptr, shape=(1033,)).astype(np.float32)

        board = torch.from_numpy(arr[:1024]).view(1, 32, 32).to(device)
        metadata = torch.from_numpy(arr[1024:]).to(device)
        return board, metadata

    # Converts a string to an action
    def stringToAction(self,state:EBoardP,str): #FIXME: ritorna un intero
        return self.stringToAction_low(state,str.encode())

    def get_mask(self, state):          # mask for actions
        ptr = self.getMask_low(state)
        return np.ctypeslib.as_array(ptr, shape=(self.MAX_ACTIONS,)).astype(bool)

    def get_actions(self,state):   # all actions masked by getMask
        ptr = self.getAssociatedAction_low(state)
        return np.ctypeslib.as_array(ptr, shape=(self.MAX_ACTIONS,)).astype(np.uint64)

    def compute_actions(self,state:EBoardP):
        self.compute_possible_low(state)

    def get_opponent_value(self, value):
        return -value

    def getActions(self,state:EBoardP):
        self.compute_actions(state)
        mask=self.get_mask(state)
        actions=self.get_actions(state)
        return actions[mask]

    def get_opponent(self, player): # TODO: il player è 1 o -1
        return -player

    def actionToString(self,action:actionT,state:EBoardP):
        self.actionToStringLow(action,state,self.strBuff)
        s=b""
        i=0
        while self.strBuff[i]!=b'\x00' and i<20:
            s+=self.strBuff[i]
            i+=1
        return s.decode()

GR=DLLGameRule()
DEBUG=False
interactionProgram=""
def initNewGame(player,gamemode=0):
    global interactionProgram
    interactionProgram=""
    match gamemode:
        case 0:
            b="Base"
        case 1:
            b="Base+M"
        case 2:
            b="Base+L"
        case 3:
            b="Base+P"
        case 4:
            b="Base+ML"
        case 5:
            b="Base+MP"
        case 6:
            b="Base+LP"
        case 7:
            b="Base+MLP"
            
        
    player.stdin.write("newgame "+b+"\n")
    s="newgame "+b+"\n"
    if DEBUG:
        interactionProgram+=s
    s=player.stdout.readline()
    if DEBUG:
        interactionProgram+=" < "+s+"\n"
    s=player.stdout.readline()
    if DEBUG:
        interactionProgram+=" < "+s+"\n"
    
def getValidMoves(player):
    global interactionProgram
    player.stdin.write("validmoves\n")
    if DEBUG:
        interactionProgram+=" > validmoves\n"
    read=player.stdout.readline()
    s=read[:-1].split(";")  
    if DEBUG:
        interactionProgram+=" <:VM "+read+"\n"
    st=player.stdout.readline()
    if DEBUG:
        interactionProgram+=" < "+st+"\n"
    return s

def playMove(p):
    global interactionProgram
    global risAct
    player.stdin.write("play "+p+"\n")
    if DEBUG:
        interactionProgram+=" > play "+p+"\n"
    risAct=player.stdout.readline()
    if DEBUG:
        interactionProgram+=" < "+risAct
    checkOk=player.stdout.readline()
    if DEBUG:
        print("NO")
        interactionProgram+=" < "+checkOk
    if checkOk=="ok\n":
        return 0
    return checkOk
def playBoth(p,state,act):
    playMove(act)
    GR.next_state(state,GR.stringToAction(state,act))
    


initNewGame(player,1)
state = GR.init_state(1)
va=GR.getActions(state)
ac=[GR.actionToString(va[i],state) for i in range(1,va[0]+1)]
ac[0]
GR.next_state(state,va[1])
GR.PrintState(state)

evN=0
v1=[]
v2=[]
v2r=[]
moves="";
allVe=[]

mode2=True
state = None
for Q in range(2000):
    
    actStrTE=[]
    actStrC=[]
    actStrE=[]
    actNumE=[]

    actNumC=[]
    geConf=""
    state = GR.init_state(0)
    initNewGame(player,1)

    iM = 0
    totMov = 0
    while iM <8000:

        actNumC = GR.getActions(state)
        #actNumC=[actTMPC[i] for i in range(1,actTMPC[0]+1)]
        actStrC=[GR.actionToString(x,state) for x in actNumC]
        #actNumC.sort()
        #actStrC.sort()

        #actStrTE = getValidMoves(player)
        #lE=len(actStrTE)
        lC=len(actNumC)

        if False and lE!=lC:
            print("ERROR: Different lenght ",iM)
            print(geConf)
            break
        """
        if not mode2:
            actStrTE = getValidMoves(player)
            actNumE=[GR.stringToAction(state,mov) for mov in actStrTE]

            actStrE=[GR.actionToString(x,state) for x in actNumE]
            actNumE.sort()
            actStrE.sort()


        lE=len(actNumE)
        lC=len(actNumC)
        if lE!=lC and not mode2:
            print("ERROR: Different lenght")
            v1=[GR.actionToString(x,state) for x in actNumE]
            v2=[GR.actionToString(x,state) for x in actNumC]
            v2r=actNumC
            allVe=(actStrTE,actTMPC,actNumE,actNumC,actStrE,actStrC)
            print(geConf)
            break
        else:
            for i in range(lE):
                if(actStrC[i]!=actStrE[i]) and not mode2:
                    print("ERROR : not same actions")
                    v1=[GR.actionToString(x,state) for x in actNumE]
                    v2=[GR.actionToString(x,state) for x in actNumC]
                    allVe=(actTMPC,actStrTE,actNumE,actNumC,actStrE,actStrC)
                    v2r=actNumC
                    print(geConf)
                    break
        # scelta casuale per il secondo giocatore
        """
        m = random.randint(1, lC)
        #geConf+=(f"{iM+1}. {actStrC[m-1]}\n")

        #play move
        act=actStrC[m-1]
        ris=playMove(act)


        if(ris!=0):
            print("ERROR : not same actions, message status:_",ris,"_", len(ris))
            v1=[GR.actionToString(x,state) for x in actNumE]
            v2=[GR.actionToString(x,state) for x in actNumC]
            allVe=(actTMPC,actStrTE,actNumE,actNumC,actStrE,actStrC)
            v2r=actNumC
            print(geConf)
            break
        #print(act, GR.stringToAction(state,act),actNumC[m-1], (actNumC[m-1]%32,actNumC[m-1]//32))
        actNum=GR.stringToAction(state,act)
        #print(iM,":",act,flush=True)
        GR.next_state(state,actNum)
        #GR.PrintState(state)
        #GR.next_state(state,GR.stringToAction(state,actStrC[m-1]))

        totMov += 1
        iM += 1

        status = GR.checkStatus(state)
        if status != 1:
            print("totMoves:", iM)
            iM=100000
            break
    print("Q:",Q)
    #GR.delBoard(state)