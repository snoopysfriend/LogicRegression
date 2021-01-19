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


void Node::span(int value) {
    this->left = new Node(this, literal(value, true));
    this->right = new Node(this, literal(value, false));
    return ;
} 


Node::Node() {

}

Node::~Node() {
    if (this->left) delete this->left; 
    if (this->right) delete this->right; 
    delete this->properties;
}

Node::Node(int pi_n, SUP s) {
    this->sup = s;
    this->value = -1;
    this->Pmask = Pattern(pi_n);
    this->Nmask = Pattern(pi_n);
    this->Nmask.data.set(); // change all the data in Nmask is 1
    /*
    only want to check set is correct
    for (int i = 0; i < pi_n; i++) {
        assert(Nmask.data[i] == 1);
    }*/
    this->height = 0;
}

Node::Node(const Node* parent, int lit) {
    this->value = lit;
    this->sup = parent->sup;
    int var = lit_to_var(lit);
    //fprintf(stderr, "erase %d variable from support %d\n", var, lit);
    this->sup.var.erase(var);
    this->sup.print();

    this->left = NULL;
    this->right = NULL;

    // initialize the mask of the Node
    this->Pmask = parent->Pmask;
    this->Nmask = parent->Nmask;
	if (lit & 1) {
		this->Pmask.data[var] = 1;
	} else {
		this->Nmask.data[var] = 0;
	}
    // height of the Node
    this->height = parent->height + 1; 
}

void Node::gen_function(std::vector<int>& function) {
    int size = this->Pmask.get_size();
    for (int i = 0; i < size; i++) {
        if (this->Pmask.data[i] == 1) {
            function.push_back(literal(i, true));
        }
    } 
    for (int i = 0; i < size; i++) {
        if (this->Nmask.data[i] == 0) {
            function.push_back(literal(i, false));
        }
    } 
}

Tree::Tree() {

}

void Tree::init(int size, SUP sup) {
    this->root = new Node(size, sup);
    this->count = 0;
    this->height = 0;
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
    //fprintf(stderr, "[Simulate node] with variable %c%d\n", (parent->get_value()&1)?'+':'-', 
     //       (parent->get_value()));
    //fprintf(stderr, "Height %d\n", parent->get_height());
    int batchNum = 120;
    SUP* sup = parent->get_support(); 
    int varNum = sup->var.size();
    //fprintf(stderr, "simulate node with size %d\n", varNum);
    //assert(varNum > 0);
    /*
    if (varNum == 0) {
        fprintf(stderr, "Do not have support should be constant\n");
        parent->properties = new Properties(Constant);
        //parent->properties->constant = (output_patterns[0].data[output_indx]==0)?ZERO: ONE;
        parent->properties->constant = ZERO;
        // TODO the choose the right constant
        return ;
    }*/
    int patternNum = batchNum * varNum;
    Pattern patterns[patternNum];   
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    int idx = 0;
    for (auto v: sup->var) {
        gen_flip_random(parent, v, batchNum, patterns+batchNum*idx);
        idx++;
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
    SUP new_sup;
    int output_indx = sup->o_idx;
    new_sup.set_idx(output_indx);
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
            new_sup.var.insert(*iter);
            new_sup.piority.push_back(
                make_pair(*iter, varietyCount[i]));
        }
    }

        // sort the input variety count 
    if (new_sup.var.size() == 0) {
        parent->properties = new Properties(Constant);
        parent->properties->constant = (output_patterns[0].data[output_indx]==0)?ZERO: ONE;
    } else {
        parent->properties = new Properties(Variation);
        sort(new_sup.piority.begin(), new_sup.piority.end(), 
                    [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                    { return  a.second > b.second;});
        parent->properties->variation = new_sup.piority[0];
        new_sup.print();
        if (sup->var.size() == 1) { // only have one child variable non need to simulate
            //int single_var = parent->properties->variation.first;
            int single_var = *(sup->var.begin());
            parent->span(single_var);
            int index = 0;
            for (auto v: sup->var) {
                if (v == single_var) {
                    break;
                }
                index++;
            }

            Pattern pattern;

            if (patterns[0+index*batchNum].data[output_indx] == 1) {
                //assert(patterns[1+index*batchNum].data[output_indx] == 1);
                parent->left_child()->properties = new Properties(Constant);
                parent->left_child()->properties->constant = (output_patterns[0+index*batchNum].data[output_indx]==0)?ZERO: ONE;
                parent->right_child()->properties = new Properties(Constant);
                parent->right_child()->properties->constant = (output_patterns[1+index*batchNum].data[output_indx]==0)?ZERO: ONE;
                //left = output_patterns[0+index*batchNum].data;
                //right = output_patterns[1+index*batchNum].data;
            } else {
                parent->left_child()->properties = new Properties(Constant);
                parent->left_child()->properties->constant = (output_patterns[1+index*batchNum].data[output_indx]==0)?ZERO: ONE;
                parent->right_child()->properties = new Properties(Constant);
                parent->right_child()->properties->constant = (output_patterns[0+index*batchNum].data[output_indx]==0)?ZERO: ONE;
            }
            /*
            for (int i = 0; i < batchNum; i+=2) {
                pattern.data = output_patterns[i+index*batchNum].data 
                    ^ output_patterns[i+index*batchNum+1].data;
               //TODO check the constant properties of the child node
            }*/
            count += 2;
        }
    }

    return ;
}

void Tree::print() {
    printf("Onset functions\n");
    for (auto function: this->onset) {
        printf("(");
        for (auto v: function) {
            if (v&1) {
                printf("%d ", lit_to_var(v)); 
            } else {
                printf("-%d ", lit_to_var(v)); 
            }
        }
        printf(")  ");
    }
    printf("\n");
    printf("Offset functions\n");
    for (auto function: this->offset) {
        printf("(");
        for (auto v: function) {
            if (v&1) {
                printf("%d ", lit_to_var(v)); 
            } else {
                printf("-%d ", lit_to_var(v)); 
            }
        }
        printf(")  ");
    }
    printf("\n");
}

void Tree::gen_function(Node* node) {
    std::vector<int> function;
    node->gen_function(function);
    if (node->properties->constant == ZERO) {
        this->offset.push_back(function);
    } else {
        this->onset.push_back(function);
    }
}


int idx = 0;
void Tree::recurse(int len, Pattern patterns[], Pattern pattern, SUP& sup) {
    //patterns.push_back(pattern);
    if (len == 0) {
        patterns[idx] = pattern;
        idx++;
        return ;
    } else {
       auto iter = sup.var.begin();
       std::advance(iter, len-1);
       int place = *iter;
       pattern.data[place] = 0;
       recurse(len-1, patterns, pattern, sup); 
       pattern.data[place] = 1;
       recurse(len-1, patterns, pattern, sup); 
    }
}

void Tree::brute_force() {
    SUP* sup = this->root->get_support(); 
    int varNum = sup->var.size();
    assert(varNum > 0 && varNum < 18);
    fprintf(stderr, "[Brute Force] with output varibale %d and input size %d\n", sup->o_idx, varNum);

    int patternNum = 1<<varNum;
    idx = 0;
    Pattern patterns[patternNum];
    Pattern pattern;
    pattern.set_size(PI_N);
    pattern.data = 0;
    recurse(varNum, patterns, pattern, *sup);
    // initialize the pattern size
    // generate all the patterns 
    assert(idx== patternNum);

    IO.output_pattern(patternNum, patterns);
    //clock_t start = clock(); 
    IO.execute();
    //clock_t end = clock(); 
    //cout << "esclapse time: " << end-start << std::endl;
    //fprintf(stderr, "end simulate\n");
    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    Pattern output_patterns[patternNum];
    IO.gen_patterns(patternNum, patterns, output_patterns);
    // TODO gen pattern do not need to gen the input patterns !!!
    
    int output_indx = sup->o_idx;
    for (int i = 0; i < patternNum; i++) {
        std::vector<int> function;
        for (auto v: sup->var) {
            function.push_back(literal(v, patterns[i].data[v]));
        }
        if (output_patterns[i].data[output_indx] == 0) {
            this->offset.push_back(function);
        } else {
            this->onset.push_back(function);
        }
    }
    this->care = this->onset.size() < this->offset.size()? ONSET: OFFSET;
}

void Tree::unate_paradim(int height_limit) {
    std::queue<Node*> q;
    q.push(this->root);
    while (q.size() > 0) {
        Node* parent = q.front();
        q.pop();
        simulate_variation(parent);
        if (parent->properties->type == Variation && parent->get_support_size() > 1){
            //fprintf(stderr, "[Spanning]\n");
            parent->span(parent->properties->variation.first);
            count += 2;
            if (parent->get_height() < height_limit) {
                q.push(parent->left_child());
                q.push(parent->right_child());
            }
        } else {
            //fprintf(stderr, "leaf\n");
            gen_function(parent);
        }
        //fprintf(stderr, "node size %d\n", count);
    }
    this->care = this->onset.size() < this->offset.size()? ONSET: OFFSET;
} 







