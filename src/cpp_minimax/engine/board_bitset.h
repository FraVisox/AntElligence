#ifndef BOARD_BITSET_H
#define BOARD_BITSET_H
using namespace std;

#include <cassert>
#include <iostream>
#include <bitset>
#include <cstdint>
#include <immintrin.h>

class BoardBitSet {

    public:

        alignas(64) uint64_t bv[16];
        alignas(64) __m512i hi,lo;

        void printDBG();

        BoardBitSet() ;
        BoardBitSet(const __m512i&,const __m512i&) ;

        bool none() const ;

        void set(const uint64_t i,const  uint64_t v);

        bool get_bit(const uint64_t i) const;

        void updateOr(const BoardBitSet& b);

        void updateAnd(const BoardBitSet& b);


    BoardBitSet getRot(int d) const;
    BoardBitSet getRot0() const;
    BoardBitSet getRot1() const;
    BoardBitSet getRot2() const;
    BoardBitSet getRot3() const;
    BoardBitSet getRot4() const;
    BoardBitSet getRot5() const;
    void reset();

};

bool operator==(const BoardBitSet& a,const BoardBitSet &b );
BoardBitSet operator|(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator&(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator^(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator~(const BoardBitSet& a);

BoardBitSet operator<<(const BoardBitSet& a,const int x);

#endif