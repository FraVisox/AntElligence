from engineInterface import *

# Test of the starting of the game
startGame("Base")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+P")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+M")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+L")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+ML")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+LP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MLP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base;InProgress;Black[1];wB1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+M;InProgress;Black[1];wA1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+L;InProgress;Black[1];wB1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+P;InProgress;Black[1];wP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+LP;InProgress;Black[1];wL")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MP;InProgress;Black[1];wM")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+ML;InProgress;Black[1];wS1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MLP;InProgress;Black[1];wG1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base;InProgress;Black[2];wB1;bA1 wB1-;wB2 -wB1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+M;InProgress;Black[2];wA1;bS1 wA1-;wA2 -wA1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+L;InProgress;Black[2];wB1;bL wB1/;wB2 \\wB1")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+P;InProgress;Black[2];wP;bA1 wP\\;wB1 /wP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+LP;InProgress;Black[2];wL;bA1 wL/;wB1 -wL")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MP;InProgress;Black[2];wP;bM wP\\;wB1 /wP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+ML;InProgress;Black[2];wM;bA1 wM\\;wB1 /wM")
print(getBoard())
print("---------------------------------\n\n---------------------------------")
startGame("Base+MLP;InProgress;Black[2];wP;bA1 wP\\;wB1 /wP")
print(getBoard())
print("---------------------------------\n\n---------------------------------")

#Test of the moves
print("\n\n\n\n\n")
startGame("Base+MLP")

playMove("wP")
print(getBoard())

playMove("bA1 wP-")
print(getBoard())

playMove("wQ -wP")
print(getBoard())

playMove("bQ bA1\\")
print(getBoard())

undo(15)
print(getBoard())

startGame("Base+MLP;BlackWins;White[7];wP;bP wP-;wQ \\wP;bQ bP-;wA1 \\wQ;bA1 bQ-;wS1 -wQ;bA2 bQ/;wG1 /wQ;bA1 \\bP;wG2 /wG1;bA2 \\bA1")
print(getBoard())