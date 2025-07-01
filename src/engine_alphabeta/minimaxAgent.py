
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
    def __init__(self,typeGame=0,weight=(ctypes.c_double * 6)(0,0,0,1,-2,30),depth=4, listmoves = None) -> None:
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
        if listmoves is not None:
            for move in listmoves:
                self.playmove(move)

    def undo(self, amount):  # TODO: implement, this is just stupid
        if amount <= 0:
            return
        moves = self.gameLog.split(";")[3:]
        self.reset()
        for move in moves[:-amount]:
            self.playmove(move)

        

    def gameinfo(self):
        if(self.gameTurn%2==0):
            return self.gameType+";"+self.gameState+";White["+str(self.gameTurn//2)+"]"+self.gameLog
        else:
            return self.gameType+";"+self.gameState+";Black["+str(self.gameTurn//2)+"]"+self.gameLog
        
    def validmoves(self):
        return GR.getActions(self.state)

    def set_time_limit(self, seconds):
        # TODO: change depth based on time limit
        pass # Not implemented the time limit, but it's not a problem since it will always be 5 seconds

    def set_depth_limit(self, depth):
        self.depth=depth
    
    def bestmove(self):
        act=self.bestaction()
        return GR.actionToString(act,self.state)
    
    def playmove(self,actionStr):
        print("decoding action",flush=True)
        act=GR.stringToAction(self.state,actionStr) # TODO: what happens if it is not valid?
        print("Decoded action")
        self.executeAction(act)
        self.gameLog+=";"+actionStr

    def executeAction(self,action):
        if GR.checkStatus(self.state) > 1:
            pass # TODO: what to do? error?
        GR.next_state(self.state,action)
        self.currentTurn+=1
        if GR.checkStatus(self.state) == 1:
            self.gameState="InProgress"
        elif GR.checkStatus(self.state) == 2:
            self.gameState="WhiteWins"
        elif GR.checkStatus(self.state) == 3:
            self.gameState="BlackWins"
        elif GR.checkStatus(self.state) == 4:
            self.gameState="Draw"
    
    def bestaction(self):
        self.numEval=0
        _, best_action= self.minimax(self.state,self.depth,True,self.w,currentTurn=self.currentTurn)
        return best_action
    
    def reset(self):
        self.currentTurn=0
        self.gameLog=""
        self.gameState="NotStarted"
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
