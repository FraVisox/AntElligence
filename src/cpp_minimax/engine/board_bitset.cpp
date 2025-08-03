using namespace std;

#include <cassert>
#include <iostream>
#include <bitset>
#include "board_bitset.h"

BoardBitSet::BoardBitSet() {
    for(int i=0;i<16;i++){
        bv[i]=0ull;
    }
}


bool BoardBitSet::none(){
    uint64_t r=0;
    for(int i=0;i<16;i+=4){
        r=r|bv[i]|bv[i+1]|bv[i+2]|bv[i+3];
    }
    return r==0;
}

void BoardBitSet::set(const  uint64_t i,const  uint64_t v){
    uint64_t x=(1ull) << (63-(i % 64));
    if(v==0){
        this->bv[i/64] &= ~x;
    }else{
        this->bv[i/64] |= x;
    }
}

bool BoardBitSet::get_bit(const uint64_t i) const {
    return (this->bv[i/64]>>(63-(i % 64)))&1;
}

void BoardBitSet::updateOr(const BoardBitSet& b){
    for(int i=0;i<16;i++){
        bv[i]|=b.bv[i];
    }
}

void BoardBitSet::updateAnd(const BoardBitSet& b){
    for(int i=0;i<16;i++){
        bv[i]&=b.bv[i];
    }
}

BoardBitSet BoardBitSet::getRot(int d) const {    
    BoardBitSet ba;
    switch (d){// 1,32,31,1023,992,993,0,0
        case 0:  // rot l of 
            for(int i=0;i<15;i++){
                ba.bv[i]=(bv[i]<<1) | (bv[i+1]>>63);
            }
            ba.bv[15]=(bv[15]<<1)|(bv[0]>>63);
            break;
        case 1:
            for(int i=0;i<15;i++){
                ba.bv[i]=(bv[i]<<32) | (bv[i+1]>>32);
            }
            ba.bv[15]=(bv[15]<<32) | (bv[0]>>32);
            break;
        case 2:
            for(int i=0;i<15;i++){
                ba.bv[i]=(bv[i]<<31) | (bv[i+1]>>33);
            }
            ba.bv[15]=(bv[15]<<31) | (bv[0]>>33);
            break;
        case 3:
            for(int i=1;i<16;i++){
                ba.bv[i]=(bv[i]>>1) | (bv[i-1]<<63);
            }
            ba.bv[0]=(bv[0]>>1) | (bv[15]<<63);
            break;
        case 4:
            for(int i=1;i<16;i++){
                ba.bv[i]=(bv[i]>>32) | (bv[i-1]<<32);
            }
            ba.bv[0]=(bv[0]>>32) | (bv[15]<<32);
            break;
        case 5:
            for(int i=1;i<16;i++){
                ba.bv[i]=(bv[i]>>31) | (bv[i-1]<<33);
            }
            ba.bv[0]=(bv[0]>>31) | (bv[15]<<33);
            break;
    }

    return ba;
}

void BoardBitSet::reset(){
    for(int i=0;i<16;i++){
        bv[i]=0;
    }
}

bool operator==(const BoardBitSet& a,const BoardBitSet &b ){
    uint64_t r=0;
    for(int i=0;i<16;i++){
        r|=a.bv[i]^b.bv[i];
    }
    return r==0; 
}
BoardBitSet operator|(const BoardBitSet& a,const BoardBitSet& b){
    BoardBitSet r;
    for(int i=0;i<16;i++){
        r.bv[i]=a.bv[i] | b.bv[i];
    }
    return r;
}

BoardBitSet operator&(const BoardBitSet& a,const BoardBitSet& b){
    BoardBitSet r;
    for(int i=0;i<16;i++){
        r.bv[i]=a.bv[i]& b.bv[i];
    }
    return r;
}

BoardBitSet operator^(const BoardBitSet& a,const BoardBitSet& b){
    BoardBitSet r;
    for(int i=0;i<16;i++){
        r.bv[i]=a.bv[i]^ b.bv[i];
    }
    return r;
}

BoardBitSet operator~(const BoardBitSet& a){
    BoardBitSet r;
    for(int i=0;i<16;i++){
        r.bv[i]=~a.bv[i];
    }
    return r;
}



void BoardBitSet::printDBG(){
    for(int i=16;i<48;i++){
        printf("%4d",(32*i+16)&1023);
        if(i<10)cout<<" ";
        for(int j=16;j<i;j++)std::cout<<"  ";
        for(int j=16;j<48;j++){
            cout<<((get_bit((j+32*i)&1023))?" 1 ":" . ");
            cout<<" ";
        }
        cout<<endl;
    }
}




