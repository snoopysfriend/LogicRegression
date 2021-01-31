#ifndef DECISON_HPP
#define DECISON_HPP

#include <vector>
#include <unordered_map>
#include <set>
#include "pattern.hpp"
#include "support.hpp"
#include "hash.hpp"
#include "node.hpp"

//extern Agent IO;
inline int literal(int index, bool sign) {
    return (index<<1) + sign;
}
  
inline int lit_to_var(int lit) {
    return (lit>>1);
}



class Tree {
public:
    Tree();
    void init(int Pi_Num, SUP sup);
    ~Tree();
    int Add_child(Node*, int, SUP*);
    void unate_paradim(int limit, int minmax);
    void brute_force(SUP*);
    void brute_force2(Node*);
    void gen_function(Node* node);
    void IDAS(int limit, int);
    void print();
    void sp_flip(Node*);
    void sp_flip2(Node*);
    void SPFLIP(Node*);
    void gen_pattern(SUP*, unordered_map<vector<int>, bool, Hash, equal_f>*, int&);
    inline CARE get_care() {
        return care;
    }
    inline Node* get_root() {
        return root;
    }
    inline std::vector<std::vector<int>>* get_function(CARE care) {
        if (care == ONSET) {
            return &onset;
        } else {
            return &offset;
        }
    }
    inline std::vector<std::vector<int>>* get_function() {
        if (care == ONSET) {
            return &onset;
        } else {
            return &offset;
        }
    }

private: 
    unordered_map<Node*, bool, supportHash, equal_Node> sup_table; 
    int find_variation(Node* node, Pattern* output_patterns, Pattern* input_patterns, int start, int& patterNum, int);
    int gen_simulate_pattern(Node* node, Pattern patterns[], int start);
    void simulate_variation(Node*, int minmax);
    void gen_flip_random(Node* node, int bit_place, int batchNum, Pattern patterns[]);
    void recurse(int, Pattern [], Pattern, SUP&);

    std::vector<std::vector<int>> onset;
    std::vector<std::vector<int>> offset;
    Node* root;
    CARE care;
	int count;
	int height;
};

class Forest{
public:
    Forest(Tree*, int, SUP);
    void MiniMax(int);
    void merge();
private:
    Tree* MaxTree;
    Tree MinTree;
};

#endif

