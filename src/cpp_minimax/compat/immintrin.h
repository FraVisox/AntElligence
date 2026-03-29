// Portable drop-in for <immintrin.h>.
// Implements every AVX-512 / SSE4.2 symbol used by board_bitset.cpp and
// board.cpp using plain C++ (uint64_t arrays).  Behaviour is bit-for-bit
// identical to the hardware intrinsics.  Placed in compat/ so that
// -I./compat shadows the system header for the perft build only.
#pragma once
#include <cstdint>
#include <cstring>

// ── Types ──────────────────────────────────────────────────────────────────

struct __m512i { uint64_t v[8]; };
typedef uint8_t __mmask8;

// ── Construction ────────────────────────────────────────────────────────────

inline __m512i _mm512_set1_epi64(long long x) {
    __m512i r;
    for (int i = 0; i < 8; i++) r.v[i] = (uint64_t)x;
    return r;
}

// e7 = highest lane (index 7), e0 = lowest (index 0)
inline __m512i _mm512_set_epi64(long long e7, long long e6, long long e5,
                                long long e4, long long e3, long long e2,
                                long long e1, long long e0) {
    __m512i r;
    r.v[0]=(uint64_t)e0; r.v[1]=(uint64_t)e1;
    r.v[2]=(uint64_t)e2; r.v[3]=(uint64_t)e3;
    r.v[4]=(uint64_t)e4; r.v[5]=(uint64_t)e5;
    r.v[6]=(uint64_t)e6; r.v[7]=(uint64_t)e7;
    return r;
}

// ── Load / Store ─────────────────────────────────────────────────────────────

inline __m512i _mm512_load_si512(const void* p)  { __m512i r; memcpy(&r,p,64); return r; }
inline __m512i _mm512_loadu_si512(const void* p) { __m512i r; memcpy(&r,p,64); return r; }
inline void _mm512_store_si512(void* p, __m512i a)  { memcpy(p,&a,64); }
inline void _mm512_storeu_si512(void* p, __m512i a) { memcpy(p,&a,64); }

// ── Bitwise ──────────────────────────────────────────────────────────────────

inline __m512i _mm512_or_si512(__m512i a, __m512i b) {
    __m512i r; for(int i=0;i<8;i++) r.v[i]=a.v[i]|b.v[i]; return r;
}
inline __m512i _mm512_and_si512(__m512i a, __m512i b) {
    __m512i r; for(int i=0;i<8;i++) r.v[i]=a.v[i]&b.v[i]; return r;
}
inline __m512i _mm512_xor_si512(__m512i a, __m512i b) {
    __m512i r; for(int i=0;i<8;i++) r.v[i]=a.v[i]^b.v[i]; return r;
}

// ── Shifts ────────────────────────────────────────────────────────────────────

inline __m512i _mm512_slli_epi64(__m512i a, int n) {
    __m512i r; for(int i=0;i<8;i++) r.v[i]=(n>=64)?0:(a.v[i]<<n); return r;
}
inline __m512i _mm512_srli_epi64(__m512i a, int n) {
    __m512i r; for(int i=0;i<8;i++) r.v[i]=(n>=64)?0:(a.v[i]>>n); return r;
}
// Concatenate-shift (used in commented-out code, included for completeness)
inline __m512i _mm512_shldi_epi64(__m512i a, __m512i b, int n) {
    __m512i r;
    for(int i=0;i<8;i++)
        r.v[i]=(n==0)?a.v[i]:(a.v[i]<<n)|(b.v[i]>>(64-n));
    return r;
}
inline __m512i _mm512_shrdi_epi64(__m512i a, __m512i b, int n) {
    __m512i r;
    for(int i=0;i<8;i++)
        r.v[i]=(n==0)?a.v[i]:(a.v[i]>>n)|(b.v[i]<<(64-n));
    return r;
}

// ── Compare ───────────────────────────────────────────────────────────────────

inline __mmask8 _mm512_cmpneq_epi64_mask(__m512i a, __m512i b) {
    __mmask8 m=0;
    for(int i=0;i<8;i++) if(a.v[i]!=b.v[i]) m|=(1u<<i);
    return m;
}

// ── Two-source permute ────────────────────────────────────────────────────────
// idx.v[i] & 15 selects the element; bits 3:0 index into a (0-7) or b (8-15)

inline __m512i _mm512_permutex2var_epi64(__m512i a, __m512i idx, __m512i b) {
    __m512i r;
    for(int i=0;i<8;i++) {
        uint64_t sel = idx.v[i] & 15;
        r.v[i] = (sel < 8) ? a.v[sel] : b.v[sel-8];
    }
    return r;
}

// ── SSE4.2 popcount ───────────────────────────────────────────────────────────

inline int _mm_popcnt_u64(unsigned long long x) {
    return __builtin_popcountll(x);
}
