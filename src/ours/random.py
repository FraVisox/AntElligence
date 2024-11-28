from random import choice
from time import sleep
from strategy import Strategy
from board import Board

class Random(Strategy):
    def calculate_best_move(self, board: Board) -> str:
        sleep(0.5) # If I don't put this here, MzingaViewer doesn't work
        return choice(board.valid_moves.split(";"))