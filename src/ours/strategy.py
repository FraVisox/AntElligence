from abc import ABC, abstractmethod
from board import Board

# Abstract class
class Strategy(ABC):
    time_limit = 0
    depth_limit = 0

    @abstractmethod
    def calculate_best_move(self, board: Board) -> str:
        pass

    def set_time_limit(self, time: int):
        self.time_limit = time
    
    def set_depth_limit(self, depth: int):
        self.depth_limit = depth