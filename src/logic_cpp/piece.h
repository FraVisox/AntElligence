#ifndef PIECE_H
#define PIECE_H
#include <string>
#include "enums.h"

class piece{
    public:
        PlayerColor col;
        int numIncr;
        BugType kind;

        piece(enum BugType k,PlayerColor c,int=0);
        piece(string s);
        piece(int val);
        piece(){};
        string toString() const;
        int toVal() const;
};

extern piece INVALID_PIECE;

template<>
struct std::hash<piece>
{
        /**
         * \brief Hash function for piece.
         *
         * Converts the integer value of the piece to a hash. The integer value
         * is a unique identifier for the piece that can be used as a hash.
         *
         * \param p The piece to hash.
         * \return The hash value.
         */
    std::size_t operator()(const piece& p) const noexcept
    {
        return  hash<int>{}(p.toVal());
    }
};


bool operator!=(const piece& p1,const piece &p);
bool operator==(const piece& p1,const piece &p);

#endif