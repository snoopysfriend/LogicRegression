#ifndef __NODE__
#define __NODE__

#include "support.hpp"
#include "pattern.hpp"

typedef enum  {
    OFFSET,
    ONSET,
    BI
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
    Node(const Node*, SUP*, int); // constructor from parent node with value
    ~Node();
	void span(int);
	void add_child(Node*);
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
    std::vector<Node*> child;
	Pattern Pmask;
	Pattern Nmask;
    int height;

private:
    SUP sup;
    //int childNum = 0;
    Node* left = NULL;
    Node* right = NULL;
	int value;
};

#endif
