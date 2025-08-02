#pragma once
using namespace std;

#include <cassert>
#include <iostream>
#include <bitset>
#include <cstdint>

class BoardBitSet {

    public:

        uint64_t bv[16];
        void printDBG();

        BoardBitSet() ;

        bool none();

        void set(uint64_t i,uint64_t v);

        bool get_bit(uint64_t i) const;

        void updateOr(const BoardBitSet& b);

        void updateAnd(const BoardBitSet& b);

        BoardBitSet getRot(int d) const ;
    void reset();

};

bool operator==(const BoardBitSet& a,const BoardBitSet &b );
BoardBitSet operator|(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator&(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator^(const BoardBitSet& a,const BoardBitSet& b);

BoardBitSet operator~(const BoardBitSet& a);

BoardBitSet operator<<(const BoardBitSet& a,const int x);
