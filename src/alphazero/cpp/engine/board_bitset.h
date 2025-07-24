using namespace std;

#include <cassert>
#include <iostream>
#include <cstring>

class OrzoBitvector {

    private:

        uint64_t bv[16];

    public:

        OrzoBitvector() {
            memset(bv, 0, sizeof(bv));
        }



        void set_bit(uint64_t i) {
            uint64_t word_idx = i / 64;
            this->bv[word_idx] |= (1ul << (i % 64));
        }
        void set(uint64_t i,uint64_t v){
        
            uint64_t word_idx = i / 64;
            this->bv[word_idx] |= (v << (i % 64));
        }

        bool get_bit(uint64_t i) {
            uint64_t word_idx = i / 64;
            return (bool) (this->bv[word_idx] & (1 << (i % 64)));
        }

};
