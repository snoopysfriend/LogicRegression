#ifndef __HASH__
#define __HASH__

#include <random>
#include "node.hpp"

struct equal_Node{
    bool operator()(Node* node1, Node* node2) const{
        Pattern pos;
        pos.data = node1->get_mask(true).data ^ node2->get_mask(true).data;
        int size = node1->get_mask(true).get_size();
        for (int i = 0; i < size; i++) {
            if (pos.data[i] != 0) {
                return false;
            }
        }
        pos.data = node1->get_mask(false).data ^ node2->get_mask(false).data;
        for (int i = 0; i < size; i++) {
            if (pos.data[i] != 0) {
                return false;
            }
        }
        return true;

    }
};

class supportHash {
public:
    supportHash() {
        std::random_device rd;
        std::mt19937_64 eng(rd());
        std::uniform_int_distribution<size_t> distr;
        for (int i = 0; i < 128; i++) {
            this->random_var[0][i] = distr(eng);
            this->random_var[1][i] = distr(eng);
        } 
    }
    size_t operator()(Node* node) const{
        size_t hash = 0;
        Pattern pmask = node->get_mask(true);
        Pattern nmask = node->get_mask(false);
        int size = pmask.get_size();
        for (int i = 0; i < size; i++) {
            if (pmask.data[i] == 1) {
                hash ^= random_var[0][i];
            } 
            if (nmask.data[i] == 0) {
                hash ^= random_var[1][i];
            }
        } 
        return hash;
    }
private:
    size_t random_var[2][128];
};


#endif
