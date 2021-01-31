#include "pattern.hpp"

Pattern::Pattern() {
    this->data = 0;
}

Pattern::Pattern(const Pattern& p) {
    this->data = p.data;
    this->size = p.size; 
}

Pattern::Pattern(int s) {
    this->data = 0;
    this->size = s;
}

Pattern::~Pattern() {

}
void Pattern::randBitset() {
    this->data = rand();
    int len = size>>4; // divide 16 becuase each size of the 
    // random is only 16 bits in rand()
    for (int i = 0 ; i < len; i++) {
        this->data <<= 16;
        this->data |= bitset<maxsize>(rand());
    }
    return ;
}





