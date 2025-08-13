using namespace std;

#include <cassert>
#include <iostream>
#include <bitset>
#include "board_bitset.h"

const __m512i zero512 = _mm512_set1_epi64(0);

BoardBitSet::BoardBitSet() {
    hi=zero512;
    lo=zero512;
    _mm512_store_si512((__m512i*)&bv[0], zero512);
    _mm512_store_si512((__m512i*)&bv[8], zero512);
}

BoardBitSet::BoardBitSet(const __m512i& l,const __m512i& h) : hi(h),lo(l) {
    _mm512_store_si512((__m512i*)&bv[0], l);
    _mm512_store_si512((__m512i*)&bv[8], h);
}


bool BoardBitSet::none() const {
    __m512i r = _mm512_or_si512(lo, hi);
    __mmask8 lr = _mm512_cmpneq_epi64_mask(r, zero512);
    return (lr)==0;

}

void BoardBitSet::set(const  uint64_t i,const  uint64_t v){
    uint64_t w = i >> 6;              
    unsigned b = 63u - (i & 63u);     
    uint64_t mask = 1ull << b;
    if (v) bv[w] |= mask; else bv[w] &= ~mask;
    if (w < 8)  lo = _mm512_loadu_si512((const void*)&bv[0]);
    else        hi = _mm512_loadu_si512((const void*)&bv[8]);
}

bool BoardBitSet::get_bit(const uint64_t i) const {


    return (this->bv[i >> 6]>>(63-(i &63u)))&1;
}

void BoardBitSet::updateOr(const BoardBitSet& b){
    __m512i new_lo = _mm512_or_si512(this->lo, b.lo);
    __m512i new_hi = _mm512_or_si512(this->hi, b.hi);
    this->lo = new_lo; this->hi = new_hi;
    _mm512_storeu_si512((void*)&bv[0], new_lo);
    _mm512_storeu_si512((void*)&bv[8], new_hi);

}

void BoardBitSet::updateAnd(const BoardBitSet& b){
    __m512i new_lo = _mm512_and_si512(this->lo, b.lo);
    __m512i new_hi = _mm512_and_si512(this->hi, b.hi);
    this->lo = new_lo; this->hi = new_hi;
    _mm512_storeu_si512((void*)&bv[0], new_lo);
    _mm512_storeu_si512((void*)&bv[8], new_hi);


}

void BoardBitSet::reset(){
    hi=zero512;
    lo=zero512;
    _mm512_store_si512((__m512i*)&bv[0], zero512);
    _mm512_store_si512((__m512i*)&bv[8], zero512);

    for(int i=0;i<16;i++)if(bv[i]!=0 )cout<<"NO reset"<<endl;
}

bool operator==(const BoardBitSet& a,const BoardBitSet &b ){

    __mmask8 hr = _mm512_cmpneq_epi64_mask(a.lo, b.lo);
    __mmask8 lr = _mm512_cmpneq_epi64_mask(a.hi, b.hi);
    

    return (hr|lr)==0;
}

BoardBitSet operator|(const BoardBitSet& a,const BoardBitSet& b){
    __m512i rl =  _mm512_or_si512(a.lo, b.lo);
    __m512i rh =  _mm512_or_si512(a.hi, b.hi);
    BoardBitSet r(rl,rh);
    return r;
}

BoardBitSet operator&(const BoardBitSet& a,const BoardBitSet& b){
    __m512i rl =  _mm512_and_si512(a.lo, b.lo);
    __m512i rh =  _mm512_and_si512(a.hi, b.hi);
    BoardBitSet r(rl,rh);
    return r;
}

BoardBitSet operator^(const BoardBitSet& a,const BoardBitSet& b){
    __m512i rl =  _mm512_xor_si512(a.lo, b.lo);
    __m512i rh =  _mm512_xor_si512(a.hi, b.hi);
    BoardBitSet r(rl,rh);
    return r;
}

BoardBitSet operator~(const BoardBitSet& a){
    const __m512i ones = _mm512_set1_epi64(-1);
    __m512i rl =  _mm512_xor_si512(a.lo, ones);
    __m512i rh =  _mm512_xor_si512(a.hi, ones);
    BoardBitSet r(rl,rh);
    return r;
}



void BoardBitSet::printDBG()const {
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





BoardBitSet BoardBitSet::getRot(int d) const {  
    switch (d)
    {
    case 0:
        return getRot0();
    case 1:
        return getRot1();
    case 2:
        return getRot2();
    case 3:
        return getRot3();
    case 4:
        return getRot4();
    case 5:
        return getRot5();
    default:
        break;
    }

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
    
    ba.lo = _mm512_load_si512((const void*)&ba.bv[0]);
    ba.hi = _mm512_load_si512((const void*)&ba.bv[8]);


    return ba;
}




const __m512i idx_next0 = _mm512_set_epi64(8+0, 7,6,5,4,3,2,1);

BoardBitSet BoardBitSet::getRot0() const {  
    const __m512i nb0 = _mm512_permutex2var_epi64(lo, idx_next0, hi);
    const __m512i nb1 = _mm512_permutex2var_epi64(hi, idx_next0, lo);

    //__m512i out0 = _mm512_shldi_epi64(lo, nb0, 1);
    //__m512i out1 = _mm512_shldi_epi64(hi, nb1, 1);
    //BoardBitSet ba1(out0,out1);

    __m512i o1 = _mm512_or_si512(_mm512_slli_epi64(lo, 1),
                               _mm512_srli_epi64(nb0, 63));
    __m512i o2 = _mm512_or_si512(_mm512_slli_epi64(hi, 1),
                               _mm512_srli_epi64(nb1, 63));

    BoardBitSet ba2(o1,o2);

   
    return ba2;
}

BoardBitSet BoardBitSet::getRot1() const {    
    const __m512i nb0 = _mm512_permutex2var_epi64(lo, idx_next0, hi);
    const __m512i nb1 = _mm512_permutex2var_epi64(hi, idx_next0, lo);

    //__m512i out0 = _mm512_shldi_epi64(lo, nb0, 32);
    //__m512i out1 = _mm512_shldi_epi64(hi, nb1, 32);
    //BoardBitSet ba1(out0,out1);

     __m512i o1 = _mm512_or_si512(_mm512_slli_epi64(lo, 32),
                               _mm512_srli_epi64(nb0, 32));
    __m512i o2 = _mm512_or_si512(_mm512_slli_epi64(hi, 32),
                               _mm512_srli_epi64(nb1, 32));

    BoardBitSet ba2(o1,o2);


    return ba2;
}


BoardBitSet BoardBitSet::getRot2() const {    
    const __m512i nb0 = _mm512_permutex2var_epi64(lo, idx_next0, hi);
    const __m512i nb1 = _mm512_permutex2var_epi64(hi, idx_next0, lo);

    //__m512i out0 = _mm512_shldi_epi64(lo, nb0, 31);
    //__m512i out1 = _mm512_shldi_epi64(hi, nb1, 31);
    //BoardBitSet ba1(out0,out1);

     __m512i o1 = _mm512_or_si512(_mm512_slli_epi64(lo, 31),
                               _mm512_srli_epi64(nb0, 33));
    __m512i o2 = _mm512_or_si512(_mm512_slli_epi64(hi, 31),
                               _mm512_srli_epi64(nb1, 33));

    BoardBitSet ba2(o1,o2);
    

    return ba2;
}

const __m512i idx_prev3 = _mm512_set_epi64(6,5,4,3,2,1,0,15);
BoardBitSet BoardBitSet::getRot3() const {    
    const __m512i pv0 = _mm512_permutex2var_epi64(lo, idx_prev3, hi); // prev for v0
    const __m512i pv1 = _mm512_permutex2var_epi64(hi, idx_prev3, lo); // prev for v1

    //__m512i out0 = _mm512_shrdi_epi64(lo, pv0, 1);
    //__m512i out1 = _mm512_shrdi_epi64(hi, pv1, 1);
    //BoardBitSet ba1(out0,out1);

    __m512i o1 = _mm512_or_si512(_mm512_srli_epi64(lo, 1),
                                _mm512_slli_epi64(pv0, 63));
    __m512i o2 = _mm512_or_si512(_mm512_srli_epi64(hi, 1),
                                _mm512_slli_epi64(pv1, 63));
    BoardBitSet ba2(o1,o2);

    return ba2;

}

BoardBitSet BoardBitSet::getRot4() const {    
    const __m512i pv0 = _mm512_permutex2var_epi64(lo, idx_prev3, hi); // prev for v0
    const __m512i pv1 = _mm512_permutex2var_epi64(hi, idx_prev3, lo); // prev for v1

    //__m512i out0 = _mm512_shrdi_epi64(lo, pv0, 32);
    //__m512i out1 = _mm512_shrdi_epi64(hi, pv1, 32);


    __m512i o1 = _mm512_or_si512(_mm512_srli_epi64(lo, 32),
                                _mm512_slli_epi64(pv0, 32));
    __m512i o2 = _mm512_or_si512(_mm512_srli_epi64(hi, 32),
                                _mm512_slli_epi64(pv1, 32));

    BoardBitSet ba2(o1,o2);
    //BoardBitSet ba1(out0,out1);
    return ba2;
}

BoardBitSet BoardBitSet::getRot5() const {    
    const __m512i pv0 = _mm512_permutex2var_epi64(lo, idx_prev3, hi); // prev for v0
    const __m512i pv1 = _mm512_permutex2var_epi64(hi, idx_prev3, lo); // prev for v1

    //__m512i out0 = _mm512_shrdi_epi64(lo, pv0, 31);
    //__m512i out1 = _mm512_shrdi_epi64(hi, pv1, 31);

    //BoardBitSet ba1(out0,out1);
    __m512i o1 = _mm512_or_si512(_mm512_srli_epi64(lo, 31),
                                _mm512_slli_epi64(pv0, 33));
    __m512i o2 = _mm512_or_si512(_mm512_srli_epi64(hi, 31),
                                _mm512_slli_epi64(pv1, 33));
    BoardBitSet ba2(o1,o2);
   
    return ba2;
}

