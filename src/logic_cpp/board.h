#ifndef BOARD_H
#define BOARD_H

#include "gameboard.h"
#include "action.h"
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <string.h>
#include <sstream>

class Board {
    public:

    //Properties of this game
    int currentTurn;
    GameState state; //InProgress, Draw, WhiteWins, BlackWins, NotStarted
    GameType type; //Base, BASE+MLP, ...

    //Moves made
    vector<action> moves = vector<action>(); 

    //Gameboard and pieces
    gameboard G = gameboard();
    vector<piece> placedBug = vector<piece>();
    unordered_set<piece> inHandPiece = unordered_set<piece>();
    bool isPlacedWQ=false;
    bool isPlacedBQ=false;

    /**
     * Board constructor.
     *
     * Initializes the Board with the white player going first.
     * The turn number is set to 1.
     */
    Board();

    /**
     * Resets the board to its initial state.
     *
     * Sets the turn number to 1, game type to Base, and game state to NOT_STARTED.
     * Clears all recorded moves, placed bugs, and pieces in hand.
     * Resets the gameboard to its default configuration.
     */

    void reset();

    /**
     * Converts the data of the board to the corresponding GameString.
     *
     * The GameString is the most detailed representation of the game state.
     *
     * :return: GameString.
     * :rtype: char*
     */
    string toString();
    
    /**
     * Returns the current player color (white if the current turn is odd).
     *
     * @return The current player color.
     */
    PlayerColor currentColor();
    /**
     * Returns the turn number of the current player.
     *
     *
     * @return The turn number of the current player.
     * @see currentColor
     */
    int currentPlayerTurn();
    
    /**
     * Returns whether the current player has placed their queen or not.
     *
     * @return Whether the current player has placed their queen or not.
     */
    bool placedQueen();

    /**
     * Executes an action. The action can be of type MOVEMENT, PLACE, PLACEFIRST
     * or PASS. Depending on the type of the action, the bug is moved, placed
     * or removed from the board, the turn is incremented, and the
     * placed/available bug pieces are updated. If the action is a PASS, nothing
     * is done.
     *
     * \param a The action to be executed.
     */
    bool executeAction(string s);

    bool checkWin();

    bool checkSurrounding(piece p);

    /**
     * Undoes the specified amount of moves.
     *
     * :param amount: Amount of moves to undo.
     * :type amount: int
     * :raises ValueError: If there are not enough moves to undo.
     * :raises ValueError: If the game has yet to begin.
     */
    void undo(int amount);

    /**
     * Returns a vector of all possible moves for the current player.
     *
     * Rules to generate moves:
     *
     * 1. If turn == 1, then place something that is not the queen.
     * 2. If turn == 2, then place something near white.
     * 3. If i didn't place the queen after turn 3, place the queen in any valid position.
     * 4. Pieces in our hand.
     * 5. Moves.
     *
     * @return A vector of all possible moves for the current player.
     */
    vector<action> possibleMoves();

    
    /**
     * \brief Adds a piece to the player's hand.
     *
     * Inserts the given piece into the in-hand piece set, indicating that the
     * piece is available for the player to use in future actions.
     * 
     * \param p The piece to add to the player's hand.
     */
    void addPieceHand(piece p);

    /**
     * \brief Generates all possible moves for a specific bug.
     *
     * This function calculates all the possible moves for the given bug based
     * on its type. If the bug belongs to the current player and can move without
     * violating the hive rules, it delegates to specific functions to generate
     * moves relevant to the bug's type. The generated moves are added to the 
     * provided result vector.
     *
     * \param b The bug for which to generate possible moves.
     * \param res The vector to store the resulting actions/moves.
     */

    void possibleMovesBug(piece b, vector<action> &res);

    /**
     * Generates all valid moves for a Queen bug piece. The Queen can move
     * to any empty adjacent position, as long as the hive rules are not
     * violated. The generated moves are added to the provided result vector.
     *
     * \param bug The Queen bug piece for which to generate moves.
     * \param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_Queen(piece bug,vector<action> &res);

    /**
     * Generates all valid moves for a Beetle bug piece. The Beetle can move
     * to any adjacent position, both if it's empty or not
     * as long as the hive rules are not violated. The generated moves are
     * added to the provided result vector.
     *
     * \param bug The Beetle bug piece for which to generate moves.
     * \param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_Beetle(piece bug,vector<action> &res);

    /**
     * Generates all valid moves for a Grasshopper bug piece. The Grasshopper
     * can move to any position that is in the same direction as the bug
     * is currently at. The generated moves are added to the provided result
     * vector.
     * \param bug The Grasshopper bug piece for which to generate moves.
     * \param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_Grasshopper(piece bug,vector<action> &res);

    /**
     * Generates all valid moves for a Soldier Ant bug piece. The Soldier Ant
     * can move to any position that is reachable by moving one or more
     * tiles in any direction. The generated moves are added to the provided
     * result vector. The algorithm used is a breadth-first search to find
     * all reachable positions.
     *
     * \param bug The Soldier Ant bug piece for which to generate moves.
     * \param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_SoldierAnt(piece bug, vector<action> & res);


    /**
     * Generates all valid moves for a Spider bug piece. The Spider can move to
     * any position that is reachable by moving three tiles in any
     * direction. The generated moves are added to the provided result vector.
     * The algorithm used is a breadth-first search to find all reachable
     * positions.
     *
     * \param bug The Spider bug piece for which to generate moves.
     * \param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_Spider(piece bug, vector<action> & res);

    /* TODO: implement. DOES THE PILLBUG NEED TO CHECK FOR SLIDE FREE?

    Ladybug: 1 climb, then either 1 crawl or 1 climb, then 1 crawl into an unoccupied space. 
            The Ladybug must start and end its move at height 1, but it cannot be at height 
            1 at any other time while moving.
    Mosquito: if its height is 1, it has the abilities of any uncovered non-Mosquito pieces in occupied adjacent spaces. 
            If it is only adjacent to uncovered Mosquitoes, it has no abilities. 
            If its height is greater than 1, it has only the abilities of a Beetle.
    Pill bug: either 1 crawl or cause an adjacent piece at height 1  
            to perform 1 climb to the same space as the Pillbug, then 1 crawl to an unoccupied destination space.
    */    

    /**
     * Computes all the possible moves for a Pillbug piece.
     * A Pillbug can move as a queen, or it can move another adjacent piece (even of the opponent) one space, as long as it doesn't break the hive.
     * @param bug The piece to move.
     * @param res The vector where the possible moves are stored.
     */
    void possibleMoves_Pillbug(piece bug, vector<action> &res);

    /**
     * Generates all valid moves for a Mosquito bug piece. The Mosquito can
     * mimic the movement abilities of neighboring bug pieces, except other
     * Mosquitoes. If the Mosquito is not at level 1, it can only move like a Beetle.
     * It also has the ability to perform special moves if it copies a Pillbug's
     * movement abilities. The generated moves are added to the provided result
     * vector.
     *
     * @param bug The Mosquito bug piece for which to generate moves.
     * @param res The vector to store the resulting actions/moves.
     */
    void possibleMoves_Mosquito(piece bug, vector<action> &res);

    /**
     * Computes all the possible moves for a Ladybug piece.
     * A Ladybug can move three or two spaces in any direction, but the first 2 moves
     * should be over of the hive.
     * @param bug The piece to move.
     * @param res The vector where the possible moves are stored.
     */
    void possibleMoves_Ladybug(piece bug,vector<action> &res);
};


#endif