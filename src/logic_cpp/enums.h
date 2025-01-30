#ifndef ENUMS_H
#define ENUMS_H

#include <string>
using namespace std;

enum PlayerColor{
    WHITE=0,
    BLACK=1
};

/**
 * \brief Converts a PlayerColor enum to a string.
 *
 * Given a PlayerColor, this function returns the corresponding string
 * representation, which is either "b" for black or "w" for white.
 *
 * \param c The PlayerColor to convert.
 * \return The string representation of the given PlayerColor.
 * \throws string "Invalid color" if c is neither BLACK nor WHITE.
 */
string ColorToString(PlayerColor c);

/**
 * \brief Converts a string to a PlayerColor enum.
 *
 * Given a string, this function returns the corresponding PlayerColor enum
 * value, which is either BLACK or WHITE.
 *
 * \param s The string to convert.
 * \return The PlayerColor enum value of the given string.
 * \throws string "Invalid color for string "+s if s is neither "b" nor "w".
 */
PlayerColor parseColor(string s);

enum GameState{
    NOT_STARTED,
    IN_PROGRESS,
    DRAW,
    WHITE_WIN,
    BLACK_WIN
};

/**
 * \brief Converts a GameState enum to a string.
 *
 * Given a GameState enum, this function returns the corresponding GameStateString
 * value.
 *
 * \param gs The GameState enum to convert.
 * \return The GameStateString value of the given GameState enum.
 * \throws string "Invalid game state" if gs is not one of the valid GameState
 *   values.
 */
string GameStateToString(GameState gs);

/**
 * \brief Converts a string to a GameState enum.
 *
 * Given a string, this function returns the corresponding GameState enum
 * value.
 *
 * \param s The string to convert.
 * \return The GameState enum value of the given string.
 * \throws string "Not valid game state for string "+s if s is not one of the
 *   valid GameStateString values.
 */
GameState parseState(string s);

enum GameType{
    Base,
    Base_M,
    Base_L,
    Base_P,
    Base_ML,
    Base_MP,
    Base_LP,
    Base_MLP
};



/**
 * \brief Converts a GameType enum to a string.
 *
 * Given a GameType enum, this function returns the corresponding string
 * representation, which describes the base and any expansions included.
 *
 * \param gt The GameType enum to convert.
 * \return The string representation of the given GameType.
 * \throws string "Invalid game type" if gt is not one of the valid GameType
 *   values.
 */
string GameTypeToString(GameType gt);


/**
 * \brief Converts a string representation of a GameType to the corresponding
 *   enum.
 *
 * Given a string that represents a GameType, this function returns the
 *   corresponding GameType enum value.
 *
 * \param s The string representation of the GameType, which should be one of
 *   the valid GameType values.
 * \return The GameType enum value corresponding to the given string.
 * \throws string "Invalid game type for string " + s if s is not one of the
 *   valid GameType string values.
 */
GameType parseGameType(string s);


enum BugType{
    QUEEN=0,
    SPIDER=1,
    BEETLE=2,
    GRASSHOPPER=3,
    SOLDIER_ANT=4,
    MOSQUITO=5,
    LADYBUG=6,
    PILLBUG=7
};


/**
 * \brief Converts a BugType to its corresponding string representation.
 *
 * Given a BugType enumeration value, this function returns the single
 * character string that represents the bug type.
 *
 * \param t The BugType to be converted to a string.
 * \return A string representing the BugType.
 */
string BugTypeToString(BugType t);

/**
 * \brief Converts a string representation of a bug type to its enumeration value.
 *
 * Given a single character string representing a bug type, this function returns
 * the corresponding BugType enumeration value.
 *
 * \param s The string to be converted to a BugType.
 * \return A BugType enumeration value.
 * \throw string Not valid bug type. If the input string is not a valid bug type.
 */
enum BugType parseBugType(string s);


#endif