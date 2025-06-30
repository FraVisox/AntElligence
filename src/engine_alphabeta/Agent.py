import ctypes
from abc import ABC, abstractmethod



class game_rule(ABC):

    @abstractmethod
    def __init__():
        pass
    @abstractmethod
    def executeAction(self,action):
        pass
    
    @abstractmethod
    def bestmove(self):
        pass
    @abstractmethod
    def reset(self):
        pass
