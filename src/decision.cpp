#include <assert.h>
#include <set>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <queue>
#include <assert.h>


#include "decision.hpp"
#include "pattern.hpp"
#include "variable.hpp"
#include "support.hpp"
#include "io.hpp" 

// TODO varcount from 0 change from 2 and 1
// TODO Tree need to store functions 
// Tree need to store onset or offset
// // TODO void Tree::simulate_variation(Node* parent)
// need to finish
// Node need to store constant value 
extern Agent IO;
extern int PO_N;
extern int PI_N;

inline int literal(int index, bool sign) {
    return (index<<1) + sign;
}

void Node::span(int value) {
    left = new Node(this, literal(value, true));
    right = new Node(this, literal(value, false));
    return ;
} 


Node::Node() {

}

Node::~Node() {
    if (left) delete left; 
    if (right) delete right; 
    delete properties;
}

Node::Node(int pi_n, SUP s) {
    sup = s;
    value = -1;
    Pmask = Pattern(pi_n);
    Nmask = Pattern(pi_n);
    Nmask.data.set(); // change all the data in Nmask is 1
    /*
    only want to check set is correct
    for (int i = 0; i < pi_n; i++) {
        assert(Nmask.data[i] == 1);
    }*/
    height = 0;
}

Node::Node(const Node* parent, int lit) {
    value = lit;
    sup = parent->sup;
    int var = lit>>1;
    fprintf(stderr, "erase %d variable from support %d\n", var, lit);
    sup.var.erase(var);
    sup.print();

    left = NULL;
    right = NULL;

    // initialize the mask of the Node
    Pmask = parent->Pmask;
    Nmask = parent->Nmask;
	if (lit & 1) {
		Pmask.data[var] = 1;
	} else {
		Nmask.data[var] = 0;
	}
    // height of the Node
    height = parent->height + 1; 
}

void Node::gen_function(std::set<int>& function) {
    for (int i = 0; i < Pmask.get_size(); i++) {
        if (Pmask.data[i] == 1) {
            function.insert(literal(i, true));
        }
    } 
    for (int i = 0; i < Nmask.get_size(); i++) {
        if (Nmask.data[i] == 0) {
            function.insert(literal(i, false));
        }
    } 
}

Tree::Tree(int size, SUP sup) {
    root = new Node(size, sup);
    count = 0;
    height = 0;
}

Tree::~Tree() {
}

void Tree::gen_flip_random(Node* n, int bit_place, int batchNum, Pattern patterns[]) {
    assert(batchNum%2 == 0); // is 2's multiple
    for (int i = 0; i < batchNum; i+=2) {
        patterns[i].randBitset(); 
        patterns[i].data |= n->get_mask(true).data;
        patterns[i].data &= n->get_mask(false).data;
        patterns[i+1].data = patterns[i].data;
        patterns[i].data[bit_place].flip();
    }
}

void Tree::simulate_variation(Node* parent){

    //fprintf(stderr,"start finding\n"); 
    fprintf(stderr, "[Simulate node] with variable %c%d\n", (parent->get_value()&1)?'+':'-', 
            (parent->get_value()));
    fprintf(stderr, "Height %d\n", parent->get_height());
    int batchNum = 200;
    SUP* sup = parent->get_support(); 
    int varNum = sup->var.size();
    fprintf(stderr, "simulate node with size %d\n", varNum);
    if (varNum == 0) {
        fprintf(stderr, "Do not have support should be constant\n");
        parent->properties = new Properties(Constant);
        //parent->properties->constant = (output_patterns[0].data[output_indx]==0)?ZERO: ONE;
        parent->properties->constant = ZERO;
        // TODO the choose the right constant
        return ;
    }
    int patternNum = batchNum * varNum;
    Pattern patterns[patternNum];   
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    int i = 0;
    for (auto v: sup->var) {
        gen_flip_random(parent, v, batchNum, patterns+batchNum*i);
        i++;
    }
    IO.output_pattern(patternNum, patterns);
    //fprintf(stderr,"begin simulate\n"); 
    //clock_t start = clock(); 
    IO.execute();
    //clock_t end = clock(); 
    //cout << "esclapse time: " << end-start << std::endl;
    
    //fprintf(stderr, "end simulate\n");
    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    Pattern output_patterns[patternNum];
    //Pattern patterns2[patternNum];   
    IO.gen_patterns(patternNum, patterns, output_patterns);
    // TODO gen pattern do not need to gen the input patterns !!!
    
    //assert(patterns[0].data == patterns2[0].data);
    int varietyCount[varNum];
    SUP output;
    int output_indx = sup->o_idx;
    output.set_idx(output_indx);
    for (int i = 0; i < varNum; i++) {
        memset(varietyCount, 0, sizeof(varietyCount));
        Pattern pattern;
        for (int j = 0; j < batchNum; j+=2) {
            pattern.data = output_patterns[j+i*batchNum].data 
                ^ output_patterns[j+i*batchNum+1].data;

            varietyCount[i] += pattern.data[output_indx];
        }
        if (varietyCount[i] != 0) { // output[j] has dependcy on input[i]
            auto iter = sup->var.begin();
            std::advance(iter, i);
            output.var.insert(*iter);
            output.piority.push_back(
                make_pair(*iter, varietyCount[i]));
        }
    }

        // sort the input variety count 
    if (output.var.size() == 0) {
        parent->properties = new Properties(Constant);
        parent->properties->constant = (output_patterns[0].data[output_indx]==0)?ZERO: ONE;
    } else {
        parent->properties = new Properties(Variation);
        sort(output.piority.begin(), output.piority.end(), 
                    [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                    { return  a.second < b.second;});
        parent->properties->variation = output.piority[0];
        output.print();
    }

    return ;
}

void Tree::print() {
    for (auto function: functions) {
        printf("(");
        for (auto v: function) {
            if (v&1) {
                printf("%d ", v>>1); 
            } else {
                printf("-%d ", v>>1); 
            }
        }
        printf(")  ");
    }
    printf("\n");
}

void Tree::unate_paradim(int height_limit) {
    std::queue<Node*> q;
    q.push(root);
    while (q.size() > 0) {
        Node* parent = q.front();
        q.pop();
        simulate_variation(parent);
        if (parent->properties->type == Variation){
            fprintf(stderr, "[Spanning]\n");
            parent->span(parent->properties->variation.first);
            count += 2;
            q.push(parent->left_child());
            q.push(parent->right_child());
        } else {
            fprintf(stderr, "leaf\n");
            std::set<int> function;
            parent->gen_function(function);
            functions.push_back(function);
        }
    }
} 







