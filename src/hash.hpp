#ifndef __HASH__
#define __HASH__

#include <random>
#include <vector>
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

struct equal_f{
    bool operator()(std::vector<int> f1, std::vector<int> f2) const{
        int s1 = f1.size();
        int s2 = f2.size();
        if (s1 != s2) {
            return false;
        }
        for (int i = 0; i < s1; i++) {
            if (f1[i] != f2[i]) {
                return false;
            }
        }
        return true;
    }
};

class Hash {
public:
    Hash() {
        std::random_device rd;
        std::mt19937_64 eng(rd());
        std::uniform_int_distribution<size_t> distr;
        for (int i = 0; i < 256; i++) {
            this->random_var[i] = distr(eng);
        } 
    }
    size_t operator()(std::vector<int> f1) const{
        size_t hash = 0;
        for (size_t i = 0; i < f1.size(); i++) {
            hash ^= random_var[i]; 
        }
        return hash;
    }
private:
    size_t random_var[128*2];
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
