#include "pattern.hpp"

Pattern::Pattern() {
    data = 0;
}

Pattern::Pattern(int s) {
    size = s;
}

void Pattern::randBitset() {
    data = rand();
    int len = size<<4; // divide 16 becuase each size of the 
    // random is only 16 bits in rand()
    for (int i = 0 ; i < len; i++) {
        data <<= 16;
        data |= bitset<maxsize>(rand());
    }
    return ;
}





