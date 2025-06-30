
from game_rule import DLLGameRule 
GR=DLLGameRule()
import ctypes
from Agent import Agent


def encodeGamemode(gamemode):
    match gamemode:
        case 0:
            return "Base"
        case 1:
            return "Base+M"
        case 2:
            return "Base+L"
        case 3:
            return "Base+P"
        case 4:
            return "Base+ML"
        case 5:
            return "Base+MP"
        case 6:
            return "Base+LP"
        case 7:
            return "Base+MLP"
            



class MinimaxAgentCPP(Agent):
    gameLog=""
    gameTurn=1
    gameState=""
    gameType=""
    def __init__(self,typeGame=0,weight=(ctypes.c_double * 6)(0,0,0,1,-2,30),depth=4) -> None:
        self.w=weight
        self.currentTurn=1
        self.depth=depth
        self.gametype=typeGame
        self.numEval=0
        self.state=GR.init_state(typeGame)
        self.name="MINIMAX"
        self.gameTurn=2
        self.gameState="NotStarted"
        self.gameLog=""
        self.gameType=encodeGamemode(typeGame)

    def undo(self, amount):
        pass

    def gameinfo(self):
        if(self.gameTurn%2==0):
            return self.gameType+";"+self.gameState+";White["+str(self.gameTurn//2)+"]"+self.gameLog
        else:
            return self.gameType+";"+self.gameState+";Black["+str(self.gameTurn//2)+"]"+self.gameLog
        
    def validmoves(self):
        pass

    def set_time_limit(self, seconds):
        pass

    def set_depth_limit(self, depth):
        pass
    
    def bestmove(self):
        act=self.bestaction()
        return GR.actionToString(act,self.state)
    
    def playmove(self,actionStr):
        act=GR.stringToAction(self.state,actionStr)
        self.executeAction(act)
        self.gameLog+=";"+actionStr
        self.gameState="InProgress"
    def executeAction(self,action):
        GR.next_state(self.state,action)
        self.currentTurn+=1
    
    def bestaction(self):
        self.numEval=0
        _, best_action= self.minimax(self.state,self.depth,True,self.w,currentTurn=self.currentTurn)
        return best_action
    def reset(self):
        self.currentTurn=0
        GR.delBoard(self.state)
        self.state=GR.init_state(self.gametype)
    def delGame(self):
        self.currentTurn=0
        GR.delBoard(self.state)
        

    def minimax(self,state, depth, maximizing_player, w, alpha=-1000, beta=+1000, currentTurn=0):
        """
        Restituisce (valore, migliore_azione)
        """

        # 1) verifica immediata di terminale
        status = GR.checkStatus(state)
        if status != 1: 
            if(maximizing_player):
                return -1000,None
            else :
                return 1000,None

        # 2) cutoff di profondità → valutazione statica
        if depth == 0:
            self.numEval+=1
            return GR.calcVal(state, w), None

        actionsVect = GR.getActions(state)  # es. [n, a1, a2, ..., a_n]
        n = actionsVect[0]
        localActions=[actionsVect[i] for i in range(1,n+1)]
        best_action = localActions[0]
        
        if maximizing_player:
            max_eval = -1000
            for action in localActions:
                child = GR.copyBoard(state)
                GR.next_state(child, action)
                evalV, _ = self.minimax(child, depth-1, False, w, alpha, beta,currentTurn=currentTurn+1)
                GR.delBoard(child)

                if evalV > max_eval:
                    max_eval = evalV
                    best_action = action

                alpha = max(alpha, evalV)
                if beta <= alpha:
                    break  # β-cut

            return max_eval, best_action

        else:
            min_eval = +1000
            for action in localActions:
                child =GR.copyBoard(state)
                GR.next_state(child, action)
                evalV, _ = self.minimax(child, depth-1, True, w, alpha, beta,currentTurn=currentTurn+1)
                GR.delBoard(child)

                if evalV < min_eval:
                    min_eval = evalV
                    best_action = action

                beta = min(beta, evalV)
                if beta <= alpha:
                    break  # α-cut

            return min_eval, best_action
