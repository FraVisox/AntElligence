#include <string>
#include "piece.h"
#include <sstream>
using namespace std;

/*
    Explaining the value of a piece:
        value=col+kind*2+numIncr*14

        col: 0 for white, 1 for black
        kind: 0 for queen, 1 for spider, 2 for beetle, 3 for grasshopper, 4 for soldier ant, 5 for mosquito, 6 for ladybug, 7 for pillbug
        numIncr: 1 for the first bug of the type, 2 for the second bug of the type,.., 0 if no bugs of the same type
    
        value = -2 for invalid piece
        value = -1 for copied piece

    Example:
        white queen: 0+0*2+0*14=0
        white beetle number 1: 0+2*2+1*14=18

*/

piece INVALID_PIECE(-2);

/**
 * \brief Constructs a piece with specified bug type, color, and increment number.
 *
 * This constructor initializes a piece with the given bug type (kind),
 * player color (col), and number increment (numIncr), which indicates
 * the order of the bug among its type.
 *
 * \param k The BugType of the piece.
 * \param c The PlayerColor of the piece.
 * \param n The increment number of the piece, representing the sequence
 *          of the bug of the same type.
 */
piece::piece(BugType k,PlayerColor c,int n){
    kind=k;
    col=c;
    numIncr=n;
}

/**
 * \brief Constructs a piece from a string representation.
 *
 * This constructor initializes a piece based on a string, where the first
 * character represents the player color, the second character represents
 * the bug type, and an optional third character represents the increment
 * number indicating the order of the bug among its type.
 *
 * \param s The string representation of the piece, consisting of
 *          the color, bug type, and optionally the increment number.
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
 * \brief Constructs a piece from a integer value.
 *
 * This constructor initializes a piece based on an integer value, which
 * encodes the player color, bug type, and increment number of the piece.
 * If a value -1 is passed, the piece is initialized as a copied piece.
 *
 * The integer value is interpreted as follows:
 * - The least significant bit represents the player color (white or black).
 * - The next 3 bits represent the bug type (0-7), which is used to determine
 *   the bug type of the piece.
 * - The remaining bits represent the increment number of the bug, which
 *   indicates the order of the bug among its type.
 *
 * \param eqVal The integer value to construct the piece from.
 */
piece::piece(int eqVal){
    if (eqVal == -1) {
        numIncr = eqVal;
        return;
    }
    col=(PlayerColor)(eqVal%2);
    kind=(BugType)((eqVal%14)/2);
    numIncr=eqVal/14;
}

/**
 * \brief Returns a string representation of the piece.
 *
 * The string representation of a piece consists of the color, bug type, and
 * optionally the increment number of the piece. The color is represented by
 * a single character ('w' for white and 'b' for black), the bug type is
 * represented by a single character representing the bug type (see
 * BugTypeToString), and the increment number is represented by a single
 * digit (0-9) if it is greater than 0.
 *
 * \return A string representation of the piece.
 */
string piece::toString() const{
    if (numIncr == -1) {
        stringstream ss; 
        ss << ColorToString(col) << "M";
        return ss.str();
    }
    stringstream ss; 
    ss << ColorToString(col) << BugTypeToString(kind) << (numIncr==0?(""):(to_string(numIncr)));
    return ss.str();
}


/**
 * \brief Calculates and returns the integer value of the piece.
 *
 * This function encodes the piece's attributes (color, bug type, and increment number)
 * into a unique integer value. If the piece is a copied piece, it returns -1.
 *
 * The value is calculated as follows:
 * - The least significant bit represents the player color (0 for white, 1 for black).
 * - The next 3 bits represent the bug type, multiplied by 2.
 * - The remaining bits represent the increment number, multiplied by 14.
 *
 * \return The encoded integer value of the piece, or -1 if the piece is a copied piece.
 */
int piece::toVal() const{
    if (numIncr == -1) 
        return numIncr;
    return col+kind*2+numIncr*14;
}

/**
 * \brief Checks if two pieces are equal.
 *
 * Two pieces are considered equal if and only if their encoded integer values are equal.
 *
 * \param p1 The first piece to compare.
 * \param p The second piece to compare.
 * \return True if the pieces are equal, false otherwise.
 */
bool operator==(const piece& p1,const piece &p){
    return p1.toVal()==p.toVal();
}

/**
 * \brief Checks if two pieces are equal or one is a copied piece of the other.
 *
 * Two pieces are considered equal or one is a copied piece of the other if and only if their encoded integer values are equal, or if one is a mosquito and the other is a copied piece, or if one is a copied piece and the other is a mosquito.
 *
 * \param p1 The first piece to compare.
 * \param p The second piece to compare.
 * \return True if the pieces are equal or one is a copied piece of the other, false otherwise.
 */
bool areBugsSameOrCopied(const piece& p1,const piece &p) {
    return (p1.toVal()==p.toVal()) || (p.toVal() != -1 && p.kind == MOSQUITO && p1.toVal() == -1) || (p1.toVal() != -1 && p1.kind == MOSQUITO && p.toVal() == -1);
}

/**
 * \brief Inequality operator for pieces.
 *
 * Compares two piece objects for inequality by comparing them
 * with the equality operator and inverting the result.
 *
 * \param p1 The first piece to compare.
 * \param p The second piece to compare.
 * \return True if both pieces have different attributes, false otherwise.
 */
bool operator!=(const piece& p1,const piece &p){
    return !(p1==p);
}

/**
 * \brief Extracts a piece from a string, given a string representation of
 *        a move.
 *
 * Given a string representation of a move, this function extracts the
 * piece moved, which is the first character of the string.
 *
 * The function checks if the first character of the string is a '-', '\\'
 * or '/' character, and if so, returns the piece represented by the
 * second character of the string.
 *
 * If the first character is not one of the above, the function looks for
 * the first occurrence of '-', '\\', or '/' in the string, and returns
 * the piece represented by the substring from the beginning of the string
 * to the found character.
 *
 * If no such character is found, the function returns the piece represented
 * by the entire string.
 *
 * \param s The string representation of the move.
 * \return The piece moved, represented as a piece object.
 */
piece extractPiece(string s){
    if(s[0]=='-' || s[0]=='/' || s[0]=='\\') return piece(s.substr(1));
    long long unsigned int pos = s.find("-", 1);
    if (pos != string::npos) return piece(s.substr(0, pos));
    pos = s.find("\\", 1);
    if (pos != string::npos) return piece(s.substr(0, pos));
    pos = s.find("/", 1);
    if (pos != string::npos) return piece(s.substr(0, pos));
    return piece(s);
}