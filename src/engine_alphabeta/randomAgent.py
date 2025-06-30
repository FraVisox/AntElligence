

from game_rule import DLLGameRule 
import ctypes
import random
GR=DLLGameRule()
        


class RandomAgentCPP:
    def __init__(self,typeGame=0,weight=(ctypes.c_double * 6)(0,0,0,1,-2,30),depth=4) -> None:
        self.w=weight
        self.currentTurn=1
        self.depth=depth
        self.gametype=typeGame
        self.state=GR.init_state(typeGame)
        self.name="RANDOM"

    def bestmove(self):
        act=self.bestaction()
        return GR.actionToString(act,self.state)

    def playmove(self,actionStr):
        act=GR.stringToAction(self.state,actionStr)
        self.executeAction(act)

    def executeAction(self,action):
        GR.next_state(self.state,action)
        self.currentTurn+=1
    
    def bestaction(self):
        actionsVect = GR.getActions(self.state) 
        n = actionsVect[0]
        m = random.randint(1, n)
        return actionsVect[m]
    def reset(self):
        self.currentTurn=0
        GR.delBoard(self.state)
        self.state=GR.init_state(self.gametype)
