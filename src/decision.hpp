#ifndef DECISON_HPP
#define DECISON_HPP

#include <vector>
#include <set>
#include "pattern.hpp"
#include "support.hpp"

//extern Agent IO;

typedef enum  {
    ONSET,
    OFFSET
} CARE;

typedef enum {
    ZERO,
    ONE
} CONSTANT;

typedef enum {
    Variation, 
    Constant
} Type;

struct Properties{
    Properties(Type t) {
        type = t;
    }
    Type type;
    union {
        std::pair<int, int> variation;        
        CONSTANT constant;  
    };
};

class Node {
public:
    Node();
    Node(int, SUP); // constructor of root node
    Node(const Node*, int); // constructor from parent node with value
    ~Node();
	void span(int);
    void gen_function(std::set<int>&);

    inline SUP* get_support() {
        return &sup;
    }
    inline int get_value() {
        return value;
    }
    inline int get_height() {
        return height;
    }
    inline Pattern get_mask(bool sign) {
        if (sign) {
            return Pmask;
        } else {
            return Nmask;
        }
    }
    inline Node* left_child() {
        return left;
    }
    inline Node* right_child() {
        return right;
    }
    Properties* properties;

private:
    SUP sup;
    Node* left;
    Node* right;
	Pattern Pmask;
	Pattern Nmask;
	int value;
    size_t height;
};

class Tree {
public:
    Tree(int Pi_Num, SUP sup);
    ~Tree();
    void unate_paradim(int limit);
    void simulate_variation(Node*);
    void gen_flip_random(Node* node, int bit_place, int batchNum, Pattern patterns[]);
    void print();
private: 
    std::vector<std::set<int>> functions;
    Node* root;
    CARE care;
	size_t count;
	int height;
};

#endif

