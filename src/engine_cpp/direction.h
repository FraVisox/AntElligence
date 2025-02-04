#ifndef DIRECTION_H
#define DIRECTION_H
#include <utility>
using namespace std;
#include <string>

enum direction{
    RIGHT=0,
    DOWN_RIGHT=1,
    DOWN_LEFT=2,
    LEFT=3,
    UP_LEFT=4,
    UP_RIGHT=5,
    OVER=6,
    INVALID = -1
};


extern direction allDirections[6];

extern pair<int,int> movementCircleClockwise[7];


/**
 * \brief Concatenates a string with a direction symbol.
 *
 * Given a string `name` and a `Direction`, this function appends or prepends
 * a symbol to `name` based on the specified direction. The symbol represents
 * the relative direction on a hexagonal grid.
 *
 * \param name The base string to which the direction symbol is added.
 * \param dir The direction for which the corresponding symbol is appended or prepended to the name.
 * \return A new string with the direction symbol added to the original name.
 */

string nameDirToString(string name, direction dir);

/**
 * \brief Converts a number to its corresponding direction.
 *
 * Given a number between 0 and 5 (inclusive), this function
 * returns the corresponding direction in the order they appear
 * in the array `allDirections`.
 *
 * \param n The number representing the direction.
 * \return The direction corresponding to the number.
 */
direction numberToDirection(int n);

/**
 * \brief Calculates the opposite direction.
 *
 * Given a direction, this function returns the opposite
 * direction on a hexagonal grid.
 *
 * \param d The direction for which the opposite direction is calculated.
 * \return The direction that is opposite to the given direction.
 */

direction oppositeDir(direction d);

/**
 * \brief Calculates the difference of coordinates associated with a direction.
 *
 * Given a direction, this function returns the difference of coordinates
 * associated with that direction on a hexagonal grid.
 *
 * \param d The direction for which the difference of coordinates is calculated.
 * \return The difference of coordinates associated with the given direction.
 */
pair<int,int> associatedDifference(direction d);
/**
 * \brief Extracts the direction from a string.
 *
 * Given a string, this function extracts and returns the direction
 * associated with the string. If the string does not contain a valid
 * direction, this function returns OVER.
 *
 * \param s The string from which the direction is extracted.
 * \return The direction associated with the string.
 */
direction extractDirection(string s);
#endif