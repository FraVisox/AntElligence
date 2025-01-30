# Engine commands
Engine commands are formatted as a single command keyword followed by zero or more command parameters. To be fully UHP-compliant, a engine must implement all of the commands as specified in this section.

Most commands output one or more lines of result text, but all must output ok on a new line after completion to signal that completion back to the viewer.

### info

```
info
```

Example:

```
info
id MzingaEngine v0.10
Mosquito;Ladybug;Pillbug
ok
```

### newgame

```
newgame
newgame GameTypeString
newgame GameString
```

Examples

```
newgame
Base;NotStarted;White[1]
ok
```

```
newgame Base+MLP
Base+MLP;NotStarted;White[1]
ok
```

```
newgame Base;InProgress;White[3];wS1;bG1 -wS1;wA1 wS1/;bG2 /bG1
Base;InProgress;White[3];wS1;bG1 -wS1;wA1 wS1/;bG2 /bG1
ok
```

### play

```
play MoveString
```

Example:

```
play wS1
Base;InProgress;Black[1];wS1
ok
```

### pass
```
pass
```

### validmoves
```
validmoves
```

Example:

```
newgame
Base;NotStarted;White[1]
ok
validmoves
wS1;wB1;wG1;wA1
ok
```

### bestmove
```
bestmove time MaxTime
bestmove depth MaxDepth
```

Example:

```
newgame
Base;NotStarted;White[1]
ok
bestmove time 00:00:05
wS1
ok
```

### undo
```
undo [MovesToUndo]
```
 
Example:

```
newgame
Base;NotStarted;White[1]
ok
play wS1
Base;InProgress;Black[1];wS1
ok
undo
Base;NotStarted;White[1]
ok
```

### options

```
options
options get OptionName
options set OptionName OptionValue
```

Example:

```
options set MaxBranchingFactor 25
MaxBranchingFactor;int;25;500;1;500
ok
```

### err
If the input is invalid:

```
hi
err Invalid command. Try 'help' to see a list of valid commands.
ok
```

Or invalidmove if the move is not valid

```
newgame Base
Base;NotStarted;White[1]
ok
validmoves
wS1;wB1;wG1;wA1
ok
play wQ
invalidmove You can't play your Queen Bee on your first turn.
ok
```

## Strings

### GameString
A GameString describes the complete state of the game. It is composed of various other strings joined by semi-colons, and is intended to include all of the information a viewer needs to display the game to the user.

```
Base;InProgress;White[2];wS1;bS1 wS1-
```

### GameTypeString
A GameTypeString identifies the expansion pieces (if any) available in the current game. All GameTypeStrings start with Base, and if there are any expansion pieces, a single + and any combination of: M for Mosquito, L for Ladybug, P for Pillbug.

```
Base
Base+M
Base+L
Base+P
Base+ML
Base+MP
Base+LP
Base+MLP
```

### GameStateString
A GameStateString represents whether the game is in progress or not, and can be any of the following values:


- NotStarted	The game has not started, no moves have been made.
- InProgress	The game is in progress, at least one move has been made.
- Draw	The game is over, the result is a draw.
- WhiteWins	The game is over, the white side has won.
- BlackWins	The game is over, the black side has won.

### TurnString
A TurnString represents which side's turn it is, as well as the turn number. The string starts with the color of the current side's turn, White or Black. After that is the turn number inside of square brackets [ and ].

```
White[1]
Black[1]
White[2]
Black[2]
```

### MoveString
The string starts with the piece to move's short name, followed by a space, followed by a position relative to another piece on the board.

The piece's short name is simply the piece's color w for white or b for black, followed by a single letter representation of the piece's bug type, and a number when a player has multiple bugs of that type, indicating the order in which the piece was initially played.

wQ / bQ
wS1 / bS1
wS2 / bS2
wB1 / bB1
wB2 / bB2
wG1 / bG1
wG2 / bG2
wG3 / bG3
wA1 / bA1
wA2 / bA2
wA3 / bA3
wM / bM
wL / bL
wP / bP

The relative position is given by listing the other piece's short name (near the destination), and (usually) prepending or appending one of three relative direction indicators: - / \

bS1 wS1/	Place the first black spider on the top right edge first white spider.
bS1 wS1-	Place the first black spider on the right-hand edge of the first white spider.
bS1 wS1\	Place the first black spider on the bottom right edge of the first white spider.
bS1 /wS1	Place the first black spider on the bottom left edge of the first white spider.
bS1 -wS1	Place the first black spider on the left-hand edge of the first white spider.
bS1 \wS1	Place the first black spider on the top left edge of the first white spider.

First move:

wS1	White started the game by placing the first white spider.

For moving pieces on top of the hive (as the beetle, or the mosquito adopting the Beetle's movement), simply state the target piece that is about to be covered.
wB1 wS1	Move the first white beetle on top of the first white spider.



