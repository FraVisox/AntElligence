#include <string>
#include "piece.h"
#include <sstream>
using namespace std;

piece INVALID_PIECE(-1);

/*
    Explaining the value of a piece:
        value=col+kind*2+numIncr*14

        col: 0 for white, 1 for black
        kind: 0 for queen, 1 for spider, 2 for beetle, 3 for grasshopper, 4 for soldier ant, 5 for mosquito, 6 for ladybug, 7 for pillbug
        numIncr: 1 for the first bug of the type, 2 for the second bug of the type,.., 0 if no bugs of the same type

    Example:
        white queen: 0+0*2+0*14=0
        white beetle number 1: 0+2*2+1*14=18

*/


/**
 * Constructor for piece.
 *
 * @param k Bug type.
 * @param c Player color.
 * @param n Number of bug of the same type.
 */
piece::piece(BugType k,PlayerColor c,int n){
    kind=k;
    col=c;
    numIncr=n;
}

/**
 * Constructs a piece object from a string representation.
 *
 * The string should start with a character representing the player color,
 * followed by a character representing the bug type, and optionally a
 * third character for the bug number if there are multiple bugs of the
 * same type.
 *
 * @param s A string representing the piece.
 */

piece::piece(string s){
    col=parseColor(s.substr(0,1));
    kind=parseBugType(s.substr(1,1));
    if(s.size()==3){
        numIncr=stoi(s.substr(2,1));
    } else{
        numIncr=0;
    }
}


/**
 * Constructs a piece object from an integer value representation.
 *
 * The integer value is decoded to extract the player color, bug type, 
 * and the number of the bug of the same type.
 *
 * @param eqVal An integer representing the piece. TODO: check if it's correct
 */
piece::piece(int eqVal){
    col=(PlayerColor)(eqVal%2);
    kind=(BugType)((eqVal%14)/2);
    numIncr=eqVal/14;
}

/**
 * Converts the piece to a string.
 *
 * The string representation of the piece is in the format
 * "<color><bug-type><number>" where <color> is a single character
 * string representing the player color, <bug-type> is a single
 * character string representing the bug type, and <number> is a
 * string representation of the number of the bug of the same type
 * if there are multiple bugs of the same type.
 *
 * @return The string representation of the piece.
 */
string piece::toString() const{
    stringstream ss; 
    ss << ColorToString(col) << BugTypeToString(kind) << (numIncr==0?(""):(to_string(numIncr)));
    return ss.str();
}

/**
 * Converts the piece to an integer value.
 *
 * The integer representation is calculated using the formula:
 * col + kind * 2 + numIncr * 14, where 'col' is the player color,
 * 'kind' is the bug type, and 'numIncr' is the number of the bug
 * of the same type.
 *
 * @return An integer representing the piece.
 */

int piece::toVal() const{
    return col+kind*2+numIncr*14;
}

/**
 * \brief Checks if two pieces are equal.
 *
 * Compares two pieces for equality by converting them to their integer
 * representations and checking if the values are equal.
 *
 * \param p1 The first piece to compare.
 * \param p The second piece to compare.
 * \return true if the pieces are equal, false otherwise.
 */

bool operator==(const piece& p1,const piece &p){
    return p1.toVal()==p.toVal();
}

/**
 * \brief Checks if two pieces are not equal.
 *
 * Compares two pieces for inequality by converting them to their integer
 * representations and checking if the values are not equal.
 *
 * \param p1 The first piece to compare.
 * \param p The second piece to compare.
 * \return true if the pieces are not equal, false otherwise.
 */
bool operator!=(const piece& p1,const piece &p){
    return !(p1==p);
}

