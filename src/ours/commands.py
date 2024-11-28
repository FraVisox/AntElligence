from enum import Enum

# Command
class Command(Enum):
    INFO = "info"
    NEWGAME = "newgame"
    PLAY = "play"
    PASS = "pass"
    VALIDMOVES = "validmoves"
    BESTMOVE = "bestmove"
    UNDO = "undo"
    OPTIONS = "options"
    EXIT = "exit"
    GET = "get"
    SET = "set"

class PieceType(Enum):
    QUEEN = "Queen"
    GRASSHOPPER = "Grasshopper"
    SPIDER = "Spider"
    BEETLE = "Beetle"
    ANT = "Ant"
    MOSQUITO = "Mosquito"
    LADYBUG = "Ladybug"
    PILLBUG = "Pillbug"

class Direction(Enum):
    LEFT_RIGHT = "-"
    PRINCIPAL_DIAGONAL = "\\"
    SECONDARY_DIAGONAL = "/"

class GameType(Enum):
    BASE = "Base"
    SEPARATOR = "+"
    M = "M"
    L = "L"
    P = "P"

class PlayerColor(Enum):
    WHITE = "White"
    BLACK = "Black"
    def get_initial(self):
        return self.value[0].lower()

class GameState(Enum):
    NOT_STARTED = "NotStarted"
    IN_PROGRESS = "InProgress"
    WHITE_WINS = "WhiteWins"
    BLACK_WINS = "BlackWins"
    DRAW = "Draw"