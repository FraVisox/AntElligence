from abc import ABC, abstractmethod
from position import Position
from board import Board

class Piece(ABC):

    is_placed = False

    def __init__(self, type, id):
        self.type = type
    
    @abstractmethod
    def moves(self, Board, position: Position):
        pass

    def place(self):
        is_placed = True

# TODO: make all the moves
class Queen(Piece):
    def moves(self, Board, position: Position):
        pass

class Ant(Piece):
    def moves(self, Board, position: Position):
        pass

class Beetle(Piece):
    def moves(self, Board, position: Position):
        pass

class Grasshopper(Piece):
    def moves(self, Board, position: Position):
        pass

class Spider(Piece):
    def moves(self, Board, position: Position):
        pass

class Mosquito(Piece):
    def moves(self, Board, position: Position):
        pass

class Ladybug(Piece):
    def moves(self, Board, position: Position):
        pass

class Pillbug(Piece):
    def moves(self, Board, position: Position):
        pass