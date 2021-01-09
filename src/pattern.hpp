#ifndef PATTERN_H
#define PATTERN_H
#include <bitset>

using namespace std;
#define maxsize 128
class Pattern {
public:
    Pattern();
    Pattern(const Pattern& p);
    Pattern(int);
    void randBitset(); // gen the random bitset with the size of bits
    bitset<maxsize> data;
    inline int get_size(){ return size; };
    inline void set_size(int s) { size = s; };
private:
    int size = -1;
};

#endif

