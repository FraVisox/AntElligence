from typing import Final
from commands import *
from position import Position

class Board():
    
    # All the other pieces are just one
    num_pieces: Final[dict[str, str]] = {
        "S" : 2,
        "B" : 2,
        "G" : 3,
        "A" : 3
    }

    pieces = {}

    def __init__(self, board_string: str):
        # Create pieces (initially none is placed, so everyone is "None"). Then they will store positions.
        if (board_string[-1] == GameType.P):
            self.pieces[PlayerColor.WHITE.get_initial(), "P"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "P"+str(i)] = Position()
        if (board_string[-2] == GameType.L or board_string[-1] == GameType.L):
            self.pieces[PlayerColor.WHITE.get_initial(), "L"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "L"+str(i)] = Position()
        if (board_string[-3] == GameType.M or board_string[-2] == GameType.M or board_string[-1] == GameType.M):
            self.pieces[PlayerColor.WHITE.get_initial(), "M"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "M"+str(i)] = Position()

        self.pieces[PlayerColor.WHITE.get_initial(), "Q"] = Position()
        self.pieces[PlayerColor.BLACK.get_initial(), "Q"] = Position()
        
        for i in range(1, self.num_pieces["S"] + 1):
            self.pieces[PlayerColor.WHITE.get_initial(), "S"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "S"+str(i)] = Position()
        
        for i in range(1, self.num_pieces["B"] + 1):
            self.pieces[PlayerColor.WHITE.get_initial(), "B"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "B"+str(i)] = Position()

        for i in range(1, self.num_pieces["G"] + 1):
            self.pieces[PlayerColor.WHITE.get_initial(), "G"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "G"+str(i)] = Position()

        for i in range(1, self.num_pieces["A"] + 1):
            self.pieces[PlayerColor.WHITE.get_initial(), "A"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "A"+str(i)] = Position()


    def move_or_place(self, move: str):
        # First 3 letters are the piece I need to move, then a space and then the position I want to move it to.
        piece = self.pieces[move[0:3]]
        if (piece == None):
            return False
        if (piece.is_placed == False):
            # Place the piece
            return self.place(piece, move[4:])
        return self.move(piece, move[4:])
    
    def parse_position(self, position: str):
        pos = self.pieces[position[0:2]]
        match position[3]:
            case Direction.LEFT_RIGHT:
                return pos.right()
            case Direction.PRINCIPAL_DIAGONAL:
                return pos.down_right()
            case Direction.SECONDARY_DIAGONAL:
                return pos.up_right()
        pos = self.pieces[position[1:3]]
        match position[0]:
            case Direction.LEFT_RIGHT:
                return pos.left()
            case Direction.PRINCIPAL_DIAGONAL:
                return pos.up_left()
            case Direction.SECONDARY_DIAGONAL:
                return pos.down_left()
    
    def move(self, piece: str, position: str):
        # TODO: make it move without breaking the hive and with the proper move
        position = self.pieces[piece]
        



    def place(self, piece: str, position: str):
        color = piece[0]
        position = self.parse_position(position)
        for p in self.pieces:
            if p.is_placed:
                if p == position:
                    return False
                if p[0] != color and p.is_near(position):
                    return False
        self.pieces[piece] = position
        return True

        

    def possible_moves(self, color: str):
        moves = ""
        for piece in self.pieces:
            if (piece[0] == color):
                moves += self.moves(piece)+";"
        return moves[:-1] #cut the final ";"
        