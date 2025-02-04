from typing import Final, Optional, Set
from engine_py.enums import GameType, GameState, PlayerColor, BugType, Direction
from engine_py.game import Position, Bug, Move
import re

class Board():
  """
  Game Board.
  """
  ORIGIN: Final[Position] = Position(0, 0)
  """
  Position of the first piece played.
  """
  NEIGHBOR_DELTAS: Final[tuple[Position, Position, Position, Position, Position, Position, Position, Position]] = (
    Position(1, 0), # Right
    Position(1, -1), # Up right
    Position(0, -1), # Up left
    Position(-1, 0), # Left
    Position(-1, 1), # Down left
    Position(0, 1), # Down right
    Position(0, 0), # Below (no change)
    Position(0, 0), # Above (no change)
  )
  """
  Offsets of every neighboring tile in each flat direction.
  """

  def __init__(self, gamestring: str = "") -> None:
    """
    Game Board instantation.

    :param gamestring: GameString, defaults to "".
    :type gamestring: str, optional
    """
    type, state, turn, moves = self._parse_gamestring(gamestring)
    self.type: Final[GameType] = type
    self.state: GameState = state
    self.turn: int = turn
    self.move_strings: list[str] = []
    self.moves: list[Optional[Move]] = []
    self._valid_moves_cache: Optional[Set[Move]] = None
    self._pos_to_bug: dict[Position, list[Bug]] = {}
    """
    Map for tile positions on the board and bug pieces placed there (pieces can be stacked).
    """
    self._bug_to_pos: dict[Bug, Optional[Position]] = {}
    """
    Map for bug pieces and their current position.  
    Position is None if the piece has not been played yet.  
    Also serves as a check for valid pieces for the game.
    """
    for color in PlayerColor:
      for expansion in self.type:
        if expansion is GameType.Base:
          self._bug_to_pos[Bug(color, BugType.QUEEN_BEE)] = None
          # Add ids greater than 0 only for bugs with multiple copies.
          for i in range(1, 3):
            self._bug_to_pos[Bug(color, BugType.SPIDER, i)] = None
            self._bug_to_pos[Bug(color, BugType.BEETLE, i)] = None
            self._bug_to_pos[Bug(color, BugType.GRASSHOPPER, i)] = None
            self._bug_to_pos[Bug(color, BugType.SOLDIER_ANT, i)] = None
          self._bug_to_pos[Bug(color, BugType.GRASSHOPPER, 3)] = None
          self._bug_to_pos[Bug(color, BugType.SOLDIER_ANT, 3)] = None
        else:
          self._bug_to_pos[Bug(color, BugType(expansion.name))] = None
    self._play_initial_moves(moves)

  def __str__(self) -> str:
    return f"{self.type};{self.state};{self.current_player_color}[{self.current_player_turn}]{';' if len(self.moves) else ''}{';'.join(self.move_strings)}"

  @property
  def current_player_color(self) -> PlayerColor:
    """
    Color of the current player.

    :rtype: PlayerColor
    """
    return list(PlayerColor)[self.turn % len(PlayerColor)]

  @property
  def current_player_turn(self) -> int:
    """
    Turn number of the current player.

    :rtype: int
    """
    return 1 + self.turn // 2

  @property
  def current_player_queen_in_play(self) -> bool:
    """
    Whether the current player's queen bee is in play.

    :rtype: bool
    """
    return bool(self._bug_to_pos[Bug(self.current_player_color, BugType.QUEEN_BEE)])

  @property
  def valid_moves(self) -> str:
    """
    Current possible legal moves in a joined list of MoveStrings.

    :rtype: str
    """
    return ";".join([self.stringify_move(move) for move in self._get_valid_moves()]) or Move.PASS

  def play(self, move_string: str) -> None:
    """
    Plays the given move.

    :param move_string: MoveString of the move to play.
    :type move_string: str
    :raises ValueError: If the game is over.
    """
    move = self._parse_move(move_string)
    if self.state is GameState.NOT_STARTED:
      self.state = GameState.IN_PROGRESS
    if self.state is GameState.IN_PROGRESS:
      self.turn += 1
      self.move_strings.append(move_string)
      self.moves.append(move)
      self._valid_moves_cache = None
      if move:
        self._bug_to_pos[move.bug] = move.destination
        if move.origin:
          self._pos_to_bug[move.origin].pop()
        if move.destination in self._pos_to_bug:
          self._pos_to_bug[move.destination].append(move.bug)
        else:
          self._pos_to_bug[move.destination] = [move.bug]
        black_queen_surrounded = (queen_pos := self._bug_to_pos[Bug(PlayerColor.BLACK, BugType.QUEEN_BEE)]) and all(self._bugs_from_pos(self._get_neighbor(queen_pos, direction)) for direction in Direction.flat())
        white_queen_surrounded = (queen_pos := self._bug_to_pos[Bug(PlayerColor.WHITE, BugType.QUEEN_BEE)]) and all(self._bugs_from_pos(self._get_neighbor(queen_pos, direction)) for direction in Direction.flat())
        if black_queen_surrounded and white_queen_surrounded:
          self.state = GameState.DRAW
        elif black_queen_surrounded:
          self.state = GameState.WHITE_WINS
        elif white_queen_surrounded:
          self.state = GameState.BLACK_WINS
    else:
      raise ValueError(f"You can't {"play" if move else Move.PASS} when the game is over")

  def undo(self, amount: int = 1) -> None:
    """
    Undoes the specified amount of moves.

    :param amount: Amount of moves to undo, defaults to 1.
    :type amount: int, optional
    :raises ValueError: If there are not enough moves to undo.
    :raises ValueError: If the game has yet to begin.
    """
    if self.state is not GameState.NOT_STARTED:
      if len(self.moves) >= amount:
        if self.state is not GameState.IN_PROGRESS:
          self.state = GameState.IN_PROGRESS
        for _ in range(amount):
          self.turn -= 1
          self.move_strings.pop()
          move = self.moves.pop()
          self._valid_moves_cache = None
          if move:
            self._pos_to_bug[move.destination].pop()
            self._bug_to_pos[move.bug] = move.origin
            if move.origin:
              self._pos_to_bug[move.origin].append(move.bug)
        if self.turn == 0:
          self.state = GameState.NOT_STARTED
      else:
        raise ValueError(f"Not enough moves to undo: asked for {amount} but only {len(self.moves)} were made")
    else:
      raise ValueError(f"The game has yet to begin")

  def stringify_move(self, move: Optional[Move]) -> str:
    """
    Returns a MoveString from the given move.

    :param move: Move.
    :type move: Optional[Move]
    :return: MoveString.
    :rtype: str
    """
    if move:
      moved: Bug = move.bug
      relative: Optional[Bug] = None
      direction: Optional[Direction] = None
      if (dest_bugs := self._bugs_from_pos(move.destination)):
        relative = dest_bugs[-1]
      else:
        for dir in Direction.flat():
          if (neighbor_bugs := self._bugs_from_pos(self._get_neighbor(move.destination, dir))) and (neighbor_bug := neighbor_bugs[0]) != moved:
            relative = neighbor_bug
            direction = dir.opposite
            break
      return Move.stringify(moved, relative, direction)
    return Move.PASS

  def _parse_turn(self, turn: str) -> int:
    """
    Parses a TurnString.

    :param turn: TurnString.
    :type turn: str
    :raises ValueError: If it's not a valid TurnString.
    :return: Turn number.
    :rtype: int
    """
    if (match := re.fullmatch(f"({PlayerColor.WHITE}|{PlayerColor.BLACK})\\[(\\d+)\\]", turn)):
      color, player_turn = match.groups()
      if (turn_number := int(player_turn)) > 0:
        return 2 * turn_number - 2 + list(PlayerColor).index(PlayerColor(color))
      else:
        raise ValueError(f"The turn number must be greater than 0")
    else:
      raise ValueError(f"'{turn}' is not a valid TurnString")

  def _parse_gamestring(self, gamestring: str) -> tuple[GameType, GameState, int, list[str]]:
    """
    Parses a GameString.

    :param gamestring: GameString.
    :type gamestring: str
    :raises TypeError: If it's not a valid GameString.
    :return: Tuple of GameString components, namely GameType, GameState, turn number, and list of moves made so far.
    :rtype: tuple[GameType, GameState, int, list[str]]
    """
    values = gamestring.split(";") if gamestring else ["", "", f"{PlayerColor.WHITE}[1]"]
    if len(values) == 1:
      values += ["", f"{PlayerColor.WHITE}[1]"]
    elif len(values) < 3:
      raise TypeError(f"'{gamestring}' is not a valid GameString")
    type, state, turn, *moves = values
    return GameType.parse(type), GameState.parse(state), self._parse_turn(turn), moves

  def _play_initial_moves(self, moves: list[str]) -> None:
    """
    Make initial moves.

    :param moves: List of MoveStrings.
    :type moves: list[str]
    :raises ValueError: If the amount of moves to make is not coherent with the turn number.
    """
    if self.turn == len(moves):
      old_turn = self.turn
      old_state = self.state
      self.turn = 0
      self.state = GameState.NOT_STARTED
      for move in moves:
        self.play(move)
      if old_turn != self.turn:
        raise ValueError(f"TurnString is not correct, should be {self.current_player_color}[{self.current_player_turn}]")
      if old_state != self.state:
        raise ValueError(f"GameStateString is not correct, should be {self.state}")
    else:
      raise ValueError(f"Expected {self.turn} moves but got {len(moves)}")

  def _get_valid_moves(self) -> Set[Move]:
    """
    Calculates the set of valid moves for the current player.

    :return: Set of valid moves.
    :rtype: Set[Move]
    """
    if not self._valid_moves_cache:
      self._valid_moves_cache = set()
      if self.state is GameState.NOT_STARTED or self.state is GameState.IN_PROGRESS:
        for bug, pos in self._bug_to_pos.items():
          # Iterate over available pieces of the current player
          if bug.color is self.current_player_color:
            # Turn 0 is White player's first turn
            if self.turn == 0:
              # Can't place the queen on the first turn
              if bug.type is not BugType.QUEEN_BEE and self._can_bug_be_played(bug):
                # Add the only valid placement for the current bug piece
                self._valid_moves_cache.add(Move(bug, None, self.ORIGIN))
            # Turn 0 is Black player's first turn
            elif self.turn == 1:
              # Can't place the queen on the first turn
              if bug.type is not BugType.QUEEN_BEE and self._can_bug_be_played(bug):
                # Add all valid placements for the current bug piece (can be placed only around the first White player's first piece)
                self._valid_moves_cache.update(Move(bug, None, self._get_neighbor(self.ORIGIN, direction)) for direction in Direction.flat())
            # Bug piece has not been played yet
            elif not pos:
              # Check hand placement, and turn and queen placement, related rule.
              if self._can_bug_be_played(bug) and (self.current_player_turn != 4 or (self.current_player_turn == 4 and (self.current_player_queen_in_play or (not self.current_player_queen_in_play and bug.type is BugType.QUEEN_BEE)))):
                # Add all valid placements for the current bug piece
                self._valid_moves_cache.update(Move(bug, None, placement) for placement in self._get_valid_placements())
            # A bug piece in play can move only if it's at the top and its queen is in play and has not been moved in the previous player's turn
            elif self.current_player_queen_in_play and self._bugs_from_pos(pos)[-1] == bug and self._was_not_last_moved(bug):
              # Can't move pieces that would break the hive. Pieces stacked upon other can never break the hive by moving
              if len(self._bugs_from_pos(pos)) > 1 or self._can_move_without_breaking_hive(pos):
                match bug.type:
                  case BugType.QUEEN_BEE:
                    self._valid_moves_cache.update(self._get_sliding_moves(bug, pos, 1))
                  case BugType.SPIDER:
                    self._valid_moves_cache.update(self._get_sliding_moves(bug, pos, 3))
                  case BugType.BEETLE:
                    self._valid_moves_cache.update(self._get_beetle_moves(bug, pos))
                  case BugType.GRASSHOPPER:
                    self._valid_moves_cache.update(self._get_grasshopper_moves(bug, pos))
                  case BugType.SOLDIER_ANT:
                    self._valid_moves_cache.update(self._get_sliding_moves(bug, pos))
                  case BugType.MOSQUITO:
                    self._valid_moves_cache.update(self._get_mosquito_moves(bug, pos))
                  case BugType.LADYBUG:
                    self._valid_moves_cache.update(self._get_ladybug_moves(bug, pos))
                  case BugType.PILLBUG:
                    self._valid_moves_cache.update(self._get_sliding_moves(bug, pos, 1))
                    self._valid_moves_cache.update(self._get_pillbug_special_moves(pos))
              else:
                match bug.type:
                  case BugType.MOSQUITO:
                    self._valid_moves_cache.update(self._get_mosquito_moves(bug, pos, True))
                  case BugType.PILLBUG:
                    self._valid_moves_cache.update(self._get_pillbug_special_moves(pos))
                  case _:
                    pass
    return self._valid_moves_cache

  def _get_valid_placements(self) -> Set[Position]:
    """
    Calculates all valid placements for the current player.

    :return: Set of valid positions where new pieces can be placed.
    :rtype: Set[Position]
    """
    placements: Set[Position] = set()
    # Iterate over all placed bug pieces of the current player
    for bug, pos in self._bug_to_pos.items():
      if bug.color is self.current_player_color and pos and self._is_bug_on_top(bug):
        # Iterate over all neighbors of the current bug piece
        for direction in Direction.flat():
          neighbor = self._get_neighbor(pos, direction)
          # If the neighboring tile is empty
          if not self._bugs_from_pos(neighbor):
            # If all neighbor's neighbors are empty or of the same color, add the neighbor as a valid placement
            if all(not self._bugs_from_pos(self._get_neighbor(neighbor, dir)) or self._bugs_from_pos(self._get_neighbor(neighbor, dir))[-1].color is self.current_player_color for dir in Direction.flat() if dir is not direction.opposite):
              placements.add(neighbor)
    return placements

  def _get_sliding_moves(self, bug: Bug, origin: Position, depth: int = 0) -> Set[Move]:
    """
    Calculates the set of valid sliding moves, optionally with a fixed depth.

    :param bug: Moving bug piece.
    :type bug: Bug
    :param origin: Initial position of the bug piece.
    :type origin: Position
    :param depth: Optional fixed depth of the move, defaults to 0.
    :type depth: int, optional
    :return: Set of valid sliding moves.
    :rtype: Set[Move]
    """
    destinations: Set[Position] = set()
    visited: Set[Position] = set()
    stack: Set[tuple[Position, int]] = {(origin, 0)}
    unlimited_depth = depth == 0
    while stack:
      current, current_depth = stack.pop()
      visited.add(current)
      if unlimited_depth or current_depth == depth:
        destinations.add(current)
      if unlimited_depth or current_depth < depth:
        stack.update(
          (neighbor, current_depth + 1)
          for direction in Direction.flat()
          if (neighbor := self._get_neighbor(current, direction)) not in visited and not self._bugs_from_pos(neighbor) and bool(self._bugs_from_pos((right := self._get_neighbor(current, direction.right_of)))) != bool(self._bugs_from_pos((left := self._get_neighbor(current, direction.left_of)))) and right != origin != left
        )
    return {Move(bug, origin, destination) for destination in destinations if destination != origin}

  def _get_beetle_moves(self, bug: Bug, origin: Position, virtual: bool = False) -> Set[Move]:
    """
    Calculates the set of valid moves for a Beetle.

    :param bug: Moving bug piece.
    :type bug: Bug
    :param origin: Initial position of the bug piece.
    :type origin: Position
    :param virtual: Whether the bug is not at origin, and is just passing by as part of its full move, defaults to False.
    :type virtual: bool, optional
    :return: Set of valid Beetle moves.
    :rtype: Set[Move]
    """
    moves: Set[Move] = set()
    for direction in Direction.flat():
      # Don't consider the Beetle in the height, unless it's a virtual move (the bug is not actually in origin, but moving at the top of origin is part of its full move).
      height = len(self._bugs_from_pos(origin)) - 1 + virtual
      destination = self._get_neighbor(origin, direction)
      dest_height = len(self._bugs_from_pos(destination))
      left_height = len(self._bugs_from_pos(self._get_neighbor(origin, direction.left_of)))
      right_height = len(self._bugs_from_pos(self._get_neighbor(origin, direction.right_of)))
      # Logic from http://boardgamegeek.com/wiki/page/Hive_FAQ#toc9
      if not ((height == 0 and dest_height == 0 and left_height == 0 and right_height == 0) or (dest_height < left_height and dest_height < right_height and height < left_height and height < right_height)):
        moves.add(Move(bug, origin, destination))
    return moves

  def _get_grasshopper_moves(self, bug: Bug, origin: Position) -> Set[Move]:
    """
    Calculates the set of valid moves for a Grasshopper.

    :param bug: Moving bug piece.
    :type bug: Bug
    :param origin: Initial position of the bug piece.
    :type origin: Position
    :return: Set of valid Grasshopper moves.
    :rtype: Set[Move]
    """
    moves: Set[Move] = set()
    for direction in Direction.flat():
      destination: Position = self._get_neighbor(origin, direction)
      distance: int = 0
      while self._bugs_from_pos(destination):
        # Jump one more tile in the same direction
        destination = self._get_neighbor(destination, direction)
        distance += 1
      if distance > 0:
        # Can only move if there's at least one piece in the way
        moves.add(Move(bug, origin, destination))
    return moves

  def _get_mosquito_moves(self, bug: Bug, origin: Position, special_only: bool = False) -> Set[Move]:
    """
    Calculates the set of valid Mosquito moves, which copies neighboring bug pieces moves, and can be either normal or special (Pillbug) moves depending on the special_only flag.

    :param bug: Mosquito bug piece.
    :type bug: Bug
    :param origin: Initial position of the bug piece.
    :type origin: Position
    :param special_only: Whether to include special moves only, defaults to False.
    :type special_only: bool, optional
    :return: Set of valid Mosquito moves.
    :rtype: Set[Move]
    """
    if len(self._bugs_from_pos(origin)) > 1:
      return self._get_beetle_moves(bug, origin)
    moves: Set[Move] = set()
    bugs_copied: Set[BugType] = set()
    for direction in Direction.flat():
      if (bugs := self._bugs_from_pos(self._get_neighbor(origin, direction))) and (neighbor := bugs[-1]).type not in bugs_copied:
         bugs_copied.add(neighbor.type)
         if special_only:
           if neighbor.type == BugType.PILLBUG:
             moves.update(self._get_pillbug_special_moves(origin))
         else:
          match neighbor.type:
            case BugType.QUEEN_BEE:
              moves.update(self._get_sliding_moves(bug, origin, 1))
            case BugType.SPIDER:
              moves.update(self._get_sliding_moves(bug, origin, 3))
            case BugType.BEETLE:
              moves.update(self._get_beetle_moves(bug, origin))
            case BugType.GRASSHOPPER:
              moves.update(self._get_grasshopper_moves(bug, origin))
            case BugType.SOLDIER_ANT:
              moves.update(self._get_sliding_moves(bug, origin))
            case BugType.LADYBUG:
              moves.update(self._get_ladybug_moves(bug, origin))
            case BugType.PILLBUG:
              moves.update(self._get_sliding_moves(bug, origin, 1))
            case BugType.MOSQUITO:
              pass
    return moves

  def _get_ladybug_moves(self, bug: Bug, origin: Position) -> Set[Move]:
    """
    Calculates the set of valid moves for a Ladybug.

    :param bug: Moving bug piece.
    :type bug: Bug
    :param origin: Initial position of the bug piece.
    :type origin: Position
    :return: Set of valid Ladybug moves.
    :rtype: Set[Move]
    """
    return {
      Move(bug, origin, final_move.destination)
      for first_move in self._get_beetle_moves(bug, origin, True) if self._bugs_from_pos(first_move.destination)
      for second_move in self._get_beetle_moves(bug, first_move.destination, True) if self._bugs_from_pos(second_move.destination) and second_move.destination != origin
      for final_move in self._get_beetle_moves(bug, second_move.destination, True) if not self._bugs_from_pos(final_move.destination) and final_move.destination != origin
    }

  def _get_pillbug_special_moves(self, origin: Position) -> Set[Move]:
    """
    Calculates the set of valid special Pillbug moves.

    :param origin: Position of the Pillbug.
    :type origin: Position
    :return: Set of valid special Pillbug moves.
    :rtype: Set[Move]
    """
    moves: Set[Move] = set()
    # There must be at least one empty neighboring tile for the Pillbug to move another bug piece
    if (empty_positions := [self._get_neighbor(origin, direction) for direction in Direction.flat() if not self._bugs_from_pos(self._get_neighbor(origin, direction))]):
      for direction in Direction.flat():
        position = self._get_neighbor(origin, direction)
        # A Pillbug can move another bug piece only if it's not stacked, it's not the last moved piece, it can be moved without breaking the hive, and it's not obstructed in moving above the Pillbug itself
        if len(bugs := self._bugs_from_pos(position)) == 1 and self._was_not_last_moved(neighbor := bugs[-1]) and self._can_move_without_breaking_hive(position) and Move(neighbor, position, origin) in self._get_beetle_moves(neighbor, position):
          moves.update(Move(neighbor, position, move.destination) for move in self._get_beetle_moves(neighbor, position, True) if move.destination in empty_positions)
    return moves

  def _can_move_without_breaking_hive(self, position: Position) -> bool:
    """
    Checks whether a bug piece can be moved from the given position.

    :param position: Position where the bug piece is located.
    :type position: Position
    :return: Whether a bug piece in the given position can move.
    :rtype: bool
    """
    # Try gaps heuristic first
    neighbors: list[list[Bug]] = [self._bugs_from_pos(self._get_neighbor(position, direction)) for direction in Direction.flat()]
    # If there is more than 1 gap, perform a DFS to check if all neighbors are still connected in some way.
    if sum(bool(neighbors[i] and not neighbors[i - 1]) for i in range(len(neighbors))) > 1:
      visited: Set[Position] = set()
      neighbors_pos: list[Position] = [pos for bugs in neighbors if bugs and (pos := self._pos_from_bug(bugs[-1]))]    
      stack: Set[Position] = {neighbors_pos[0]}
      while stack:
        current = stack.pop()
        visited.add(current)
        stack.update(neighbor for direction in Direction.flat() if (neighbor := self._get_neighbor(current, direction)) != position and self._bugs_from_pos(neighbor) and neighbor not in visited)
      # Check if all neighbors with bug pieces were visited
      return all(neighbor_pos in visited for neighbor_pos in neighbors_pos)
    # If there is only 1 gap, then all neighboring pieces are connected even without the piece at the given position.
    return True

  def _can_bug_be_played(self, piece: Bug) -> bool:
    """
    Checks whether the given bug piece can be drawn from hand (has the lowest ID among same-type pieces).

    :param piece: Bug piece to check.
    :type piece: Bug
    :return: Whether the given bug piece can be played.
    :rtype: bool
    """
    return all(bug.id >= piece.id for bug, pos in self._bug_to_pos.items() if pos is None and bug.type is piece.type and bug.color is piece.color)

  def _was_not_last_moved(self, bug: Bug) -> bool:
    """
    Checks whether the given bug piece was not moved in the previous turn.

    :param bug: Bug piece.
    :type bug: Bug
    :return: Whether the bug piece was not last moved.
    :rtype: bool
    """
    return not self.moves[-1] or self.moves[-1].bug != bug

  def _parse_move(self, move_string: str) -> Optional[Move]:
    """
    Parses a MoveString.

    :param move_string: MoveString.
    :type move_string: str
    :raises ValueError: If move_string is 'pass' but there are other valid moves.
    :raises ValueError: If move_string is not a valid move for the current board state.
    :raises ValueError: If bug_string_2 has not been played yet.
    :raises ValueError: If more than one direction was specified.
    :raises ValueError: If move_string is not a valid MoveString.
    :return: Move.
    :rtype: Optional[Move]
    """
    if move_string == Move.PASS:
      if not self._get_valid_moves():
        return None
      raise ValueError(f"You can't pass when you have valid moves")
    if (match := re.fullmatch(Move.REGEX, move_string)):
      bug_string_1, _, _, _, _, left_dir, bug_string_2, _, _, _, right_dir = match.groups()
      if not left_dir or not right_dir:
        moved = Bug.parse(bug_string_1)
        if (relative_pos := self._pos_from_bug(Bug.parse(bug_string_2)) if bug_string_2 else self.ORIGIN):
          move = Move(moved, self._pos_from_bug(moved), self._get_neighbor(relative_pos, Direction(f"{left_dir}|") if left_dir else Direction(f"|{right_dir or ""}")))
          if move in self._get_valid_moves():
            return move
          raise ValueError(f"'{move_string}' is not a valid move for the current board state")
        raise ValueError(f"'{bug_string_2}' has not been played yet")
      raise ValueError(f"Only one direction at a time can be specified")
    raise ValueError(f"'{move_string}' is not a valid MoveString")

  def _is_bug_on_top(self, bug: Bug) -> bool:
    """
    Checks if the given bug has been played and is at the top of the stack.

    :param bug: Bug piece.
    :type bug: Bug
    :return: Whether the bug is at the top.
    :rtype: bool
    """
    return (pos := self._pos_from_bug(bug)) != None and self._bugs_from_pos(pos)[-1] == bug

  def _bugs_from_pos(self, position: Position) -> list[Bug]:
    """
    Retrieves the list of bug pieces from the given position.

    :param position: Tile position.
    :type position: Position
    :return: The list of bug pieces at the given position.
    :rtype: list[Bug]
    """
    return self._pos_to_bug[position] if position in self._pos_to_bug else []

  def _pos_from_bug(self, bug: Bug) -> Optional[Position]:
    """
    Retrieves the position of the given bug piece.

    :param bug: Bug piece to get the position of.
    :type bug: Bug
    :return: Position of the given bug piece.
    :rtype: Optional[Position]
    """
    return self._bug_to_pos[bug] if bug in self._bug_to_pos else None

  def _get_neighbor(self, position: Position, direction: Direction) -> Position:
    """
    Returns the neighboring position from the given direction.

    :param position: Central position.
    :type position: Position
    :param direction: Direction of movement.
    :type direction: Direction
    :return: Neighboring position in the specified direction.
    :rtype: Position
    """
    return position + self.NEIGHBOR_DELTAS[direction.delta_index]
  
  def _get_number_of_bugs_near_queen(self, color: PlayerColor) -> int:
    """
    Returns the number of bugs near the queen of the given color.

    :param color: Color of the queen.
    :type color: PlayerColor
    :return: Number of bugs near the queen.
    :rtype: int
    """
    return 0#sum(1 for bug in self._bugs_from_pos(self._pos_from_bug(Bug(color, BugType.QUEEN_BEE))) if self._is_bug_on_top(bug))
