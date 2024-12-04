from typing import Final
from commands import *
from position import Position

class Board():

    state = GameState.NOT_STARTED
    
    # All the other pieces are just one
    num_pieces: Final[dict[str, str]] = {
        "S" : 2,
        "B" : 2,
        "G" : 3,
        "A" : 3
    }

    pieces = {} # Here the key is the couple ["b", "S1"]

    def __init__(self, board_string: str):
        # Create pieces (initially none is placed, so everyone is "None"). Then they will store positions.
        if (board_string[-1] == GameType.P):
            self.pieces[PlayerColor.WHITE.get_initial(), "P"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "P"+str(i)] = Position()
            board_string = board_string[:-1]
        if (board_string[-1] == GameType.L or board_string[-1] == GameType.L):
            self.pieces[PlayerColor.WHITE.get_initial(), "L"+str(i)] = Position()
            self.pieces[PlayerColor.BLACK.get_initial(), "L"+str(i)] = Position()
            board_string = board_string[:-1]
        if (board_string[-1] == GameType.M or board_string[-2] == GameType.M or board_string[-1] == GameType.M):
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

        print(self.pieces)


    def move_or_place(self, move: str):
        # First letters are the piece I need to move, then a space and then the position I want to move it to.
        # TODO: check input string with a regex
        move = move.split()
        piece_type = move[0]
        pos = self.pieces[piece_type[0], piece_type[1:]]
        if (pos == None): # If the piece doesn't exist
            return False
        if (pos.is_placed == False):
            # Place the piece
            return self.place(piece_type, pos, move[1])
        return self.move(piece_type, pos, move[1])
    
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
    
    def move(self, piece: str, pos: Position, new_pos: str) -> bool:
        # TODO: make it move without breaking the hive and with the proper move
        pass
        
    def place(self, piece_type: str, position: Position, newpos: str):
        color = piece_type[0]
        position = self.parse_position(newpos)
        for p in self.pieces.values():
            if p.is_placed:
                if p == position:
                    return False
                if p[0] != color and p.is_near(position) and not self.placed_over(p):
                    return False
        self.pieces[color, piece_type[1:]] = position
        if (self.state == GameState.NOT_STARTED): 
            self.state = GameState.IN_PROGRESS
        return True
    
    def placed_over(self, pos: Position):
        for p in self.pieces.values():
            if p == pos:
                continue
            if p.is_over(pos):
                return True
        return False

    def possible_moves(self, color: str):
        moves = ""
        for piece in self.pieces:
            if (piece[0] == color):
                moves += self.moves(piece)+";"
        return moves[:-1] #cut the final ";"
    
    def moves(self, piece: str):
        pos = self.pieces[piece[0], piece[1:]]
        if (pos.is_placed):
            pass
        