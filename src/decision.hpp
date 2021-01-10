#ifndef DECISON_HPP
#define DECISON_HPP

#include <vector>
#include <set>
#include "pattern.hpp"
#include "support.hpp"

//extern Agent IO;
inline int literal(int index, bool sign) {
    return (index<<1) + sign;
}
  
inline int lit_to_var(int lit) {
    return (lit>>1);
}

typedef enum  {
    OFFSET,
    ONSET
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
    void gen_function(std::vector<int>&);

    inline SUP* get_support() {
        return &sup;
    }
    inline int get_support_size() {
        return sup.var.size();
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
    int height;
};

class Tree {
public:
    Tree();
    void init(int Pi_Num, SUP sup);
    ~Tree();
    void unate_paradim(int limit);
    void brute_force();
    void recurse(int, Pattern [], Pattern, SUP&);
    void simulate_variation(Node*);
    void gen_flip_random(Node* node, int bit_place, int batchNum, Pattern patterns[]);
    void gen_function(Node* node);
    void print();
    inline CARE get_care() {
        return care;
    }
    inline Node* get_root() {
        return root;
    }
    inline std::vector<std::vector<int>>* get_function() {
        if (care == ONSET) {
            return &onset;
        } else {
            return &offset;
        }
    }

private: 
    std::vector<std::vector<int>> onset;
    std::vector<std::vector<int>> offset;
    Node* root;
    CARE care;
	int count;
	int height;
};

#endif
