from engineInterface import *

# Test of the starting of the game
startGame("Base")
startGame("Base+M")
startGame("Base+L")
startGame("Base+ML")
startGame("Base+MP")
startGame("Base+LP")
startGame("Base+MLP")
startGame("Base;InProgress;Black[1];wB1")
startGame("Base+M;InProgress;Black[1];wA1")
startGame("Base+L;InProgress;Black[1];wB1")
startGame("Base+P;InProgress;Black[1];wP")
startGame("Base+LP;InProgress;Black[1];wL")
startGame("Base+MP;InProgress;Black[1];wM")
startGame("Base+ML;InProgress;Black[1];wS1")
startGame("Base+MLP;InProgress;Black[1];wG1")
startGame("Base;InProgress;Black[2];wB1;bA1 wB1-;wB2 -wB1")
startGame("Base+M;InProgress;Black[2];wA1;bS1 wA1-;wA2 -wA1")
startGame("Base+L;InProgress;Black[2];wB1;bL wB1/;wB2 \\wB1")
startGame("Base+P;InProgress;Black[2];wP;bA1 wP\\;wB1 /wP")
startGame("Base+LP;InProgress;Black[2];wL;bA1 wL/;wB1 -wL")
startGame("Base+MP;InProgress;Black[2];wP;bM wP\\;wB1 /wP")
startGame("Base+ML;InProgress;Black[2];wM;bA1 wM\\;wB1 /wM")
startGame("Base+MLP;InProgress;Black[2];wP;bA1 wP\\;wB1 /wP")

#Test of the moves
playMove("wP")
print(getBoard())

playMove("bA1 wP-")
print(getBoard())

playMove("wQ -wP")
print(getBoard())

playMove("bQ bA1\\")
print(getBoard())

#playMove("wS1 wQ/")

#playMove("bA1 bQ-")

#playMove("wL wS1/")

#playMove("bL bA1-")

#playMove("wL wP\\")
#playMove("bL bP\\")