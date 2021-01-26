#include <assert.h>
#include <set>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <queue>
#include <assert.h>
#include <unordered_map>


#include "decision.hpp"
#include "pattern.hpp"
#include "variable.hpp"
#include "support.hpp"
#include "io.hpp" 
#include "hash.hpp"

#define batchSize  240
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


void Node::add_child(Node* node) {
    this->child.push_back(node);
}


Node::Node() {

}

Node::~Node() {
    if (this->left) delete this->left; 
    if (this->right) delete this->right; 
    //delete this->properties;
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

Node::Node(const Node* parent, SUP* new_sup, int lit) {
    this->value = lit;
    this->sup = *new_sup;
    int var = lit_to_var(lit);
    //printf("remove %d\n", lit);
    this->sup.var.erase(var);
    //this->sup.print();

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

Node::Node(const Node* parent, int lit) {
    this->value = lit;
    this->sup = parent->sup;
    int var = lit_to_var(lit);
    //fprintf(stderr, "erase %d variable from support %d\n", var, lit);
    this->sup.var.erase(var);
    //this->sup.print();

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

int Tree::gen_simulate_pattern(Node* node, Pattern patterns[], int start) {
    int batchNum = batchSize;
    SUP* sup = node->get_support(); 
    int varNum = sup->var.size();
    assert(varNum > 0);

    int patternNum = batchNum * varNum;
    //printf("start simulate with size %d\n", patternNum);

    for (int i = start; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    int idx = 0;
    for (auto var: sup->var) {
        for (int i = start + idx*batchNum; i < start + (idx+1)*batchNum; i+=2) {
            patterns[i].randBitset(); 
            patterns[i].data |= node->get_mask(true).data;
            patterns[i].data &= node->get_mask(false).data;
            patterns[i+1].data = patterns[i].data;
            patterns[i].data[var].flip();
        }
        idx++;
    }
    return start + patternNum;
}

int min(int a, int b) {
    return a<b?a:b;
}

int Tree::find_variation(Node* node, Pattern* output_patterns, Pattern* patterns, int start, int& patternNum, int minimax) {

    int batchNum = batchSize;
    SUP* sup = node->get_support(); 
    int varNum = sup->var.size();
    int varietyCount[varNum];

    SUP new_sup;
    int output_indx = sup->o_idx;
    new_sup.set_idx(output_indx);

    for (int i = 0; i < varNum; i++) {
        memset(varietyCount, 0, sizeof(varietyCount));
        Pattern pattern;
        for (int j = 0; j < batchNum; j+=2) {
            pattern.data = output_patterns[j+i*batchNum+start].data 
                ^ output_patterns[j+i*batchNum+1 + start].data;

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

    if (new_sup.var.size() == 0) {
        node->properties = new Properties(Constant);
        node->properties->constant = (output_patterns[start].data[output_indx]==0)?ZERO: ONE;
    } else {
        node->properties = new Properties(Variation);
        if (minimax) {
            sort(new_sup.piority.begin(), new_sup.piority.end(), 
                        [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                        { return  a.second > b.second;});
            node->properties->variation = new_sup.piority[0];
        } else {
            sort(new_sup.piority.begin(), new_sup.piority.end(), 
                        [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                        { return  a.second > b.second;});
            node->properties->variation = new_sup.piority[0];
        }

        if (sup->var.size() == 1) { // only have one child variable non need to simulate
            int single_var = *(sup->var.begin());
            node->span(single_var);
            int index = 0;
            /*for (auto v: sup->var) {
                if (v == single_var) {
                    break;
                }
                index++;
            }*/
            assert(index == 0);

            Pattern pattern;

            if (patterns[start].data[single_var] == 1) {
                node->left_child()->properties = new Properties(Constant);
                node->left_child()->properties->constant = (output_patterns[start].data[output_indx]==0)?ZERO: ONE;
                node->right_child()->properties = new Properties(Constant);
                node->right_child()->properties->constant = (output_patterns[start+1].data[output_indx]==0)?ZERO: ZERO;
            } else {
                node->left_child()->properties = new Properties(Constant);
                node->left_child()->properties->constant = (output_patterns[start+1].data[output_indx]==0)?ZERO: ONE;
                node->right_child()->properties = new Properties(Constant);
                node->right_child()->properties->constant = (output_patterns[start].data[output_indx]==0)?ZERO: ONE;
            }
            this->count += 2;
        } else {
            //int size = new_sup.var.size();
            int size = new_sup.var.size();
            int support_size = sup->var.size();
            for (int i = 0; i < 1; i++) {
                int child_num = this->Add_child(node, new_sup.piority[i].first, sup); 
                if (child_num > 0) {
                    this->count += child_num;
                    patternNum += batchNum * 2 * (support_size-1);
                    //this->count += 2;
                }
                /*child_num = this->Add_child(node, new_sup.piority[size-1].first, sup); 
                if (child_num > 0) {
                    this->count += child_num;
                    patternNum += batchNum * 2 * (support_size-1);
                    this->count += 2;
                }*/
            }
        }
    }
    //fprintf(stderr, "patternNum %d\n", patternNum);
    
    return start + batchNum * varNum;
}

int Tree::Add_child(Node* node, int var, SUP* sup) {

    Node* pos = new Node(node, sup, literal(var, true)); 
    if (this->sup_table.find(pos) != sup_table.end()) { // exists 
        //printf("find before\n");
        //assert(false);
        delete pos;
        return 0;
    } 
    node->add_child(pos);
    sup_table[pos] = true;
    Node* neg = new Node(node, sup, literal(var, false)); 
    node->add_child(neg);
    sup_table[neg] = true;
    
    return 2;
}


void Tree::simulate_variation(Node* parent, int minmax) {

    int batchNum = batchSize;
    SUP* sup = parent->get_support(); 
    int varNum = sup->var.size();
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
    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    Pattern output_patterns[patternNum];
    //Pattern patterns2[patternNum];   
    IO.gen_patterns(patternNum, patterns, output_patterns);
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
        if (minmax) {
            sort(new_sup.piority.begin(), new_sup.piority.end(), 
                        [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                        { return  a.second > b.second;});
        } else {
            sort(new_sup.piority.begin(), new_sup.piority.end(), 
                        [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                        { return  a.second < b.second;});
        }
        parent->properties->variation = new_sup.piority[0];
        if (new_sup.var.size() == 1) { // only have one child variable non need to simulate
            int single_var = *(new_sup.var.begin());
            parent->span(single_var);
            int index = 0;
            for (auto var: sup->var) {
                if (var == single_var) {
                    break;
                }
                index++;
            }

            Pattern pattern;

            if (patterns[0+index*batchNum].data[output_indx] == 1) {
                parent->left_child()->properties = new Properties(Constant);
                parent->left_child()->properties->constant = (output_patterns[0+index*batchNum].data[output_indx]==0)?ZERO: ONE;
                parent->right_child()->properties = new Properties(Constant);
                parent->right_child()->properties->constant = (output_patterns[1+index*batchNum].data[output_indx]==0)?ZERO: ONE;
            } else {
                parent->left_child()->properties = new Properties(Constant);
                parent->left_child()->properties->constant = (output_patterns[1+index*batchNum].data[output_indx]==0)?ZERO: ONE;
                parent->right_child()->properties = new Properties(Constant);
                parent->right_child()->properties->constant = (output_patterns[0+index*batchNum].data[output_indx]==0)?ZERO: ONE;
            }
            this->count += 2;
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

void Tree::unate_paradim(int height_limit, int minmax) {
    std::queue<Node*> q;
    q.push(this->root);
    int max_height = 0;
    while (q.size() > 0) {
        Node* parent = q.front();
        q.pop();
        simulate_variation(parent, minmax);
        if (parent->properties->type == Variation && parent->get_support_size() > 1){
            //fprintf(stderr, "[Spanning]\n");
            parent->span(parent->properties->variation.first);
            count += 2;
            if (parent->get_height() > max_height) {
                max_height = parent->get_height();
                fprintf(stderr, "new height %d\n", max_height);
            }
            if (parent->get_height() < height_limit) {
                q.push(parent->left_child());
                q.push(parent->right_child());
            }
        } else {
            //fprintf(stderr, "leaf\n");
            gen_function(parent);
        }
        if (count % 100 == 0) {
            fprintf(stderr, "node size %d\n", count);
        }
    }
    this->care = this->onset.size() < this->offset.size()? ONSET: OFFSET;
} 

void Tree::IDAS(int height_limit, int minmax) {
    //std::queue<Node*> q;
    //q.push(this->root);
    unordered_map<SUP, bool, supportHash> sup_table; 
    printf("begin using IDAS\n");
    int max_height = 0;
    vector<Node*> layer;
    layer.push_back(this->root);
    int patternNum = this->root->get_support()->var.size() * batchSize;
    while (max_height < height_limit) {
        if (patternNum == 0) {
            break; 
        }
        Pattern* patterns = new Pattern[patternNum]; // gen the output pattern
        int start = 0;
        for (auto node: layer) {
            start = gen_simulate_pattern(node, patterns, start);
        } 
        fprintf(stderr,"IDAS with pattern %d\n", patternNum);
        // IO simulate it 
        IO.output_pattern(patternNum, patterns); // output the pattern to the file
        IO.execute(); // use iogen to read the file to gen the pattern
        fprintf(stderr, "end simulate\n");

        int s_patternNum = IO.read_relation();
        assert(s_patternNum == patternNum);
        Pattern* output_patterns = new Pattern[patternNum];
        IO.gen_patterns(patternNum, patterns, output_patterns);

        vector<Node*> next_layer;
        next_layer.clear();
        start = 0;
        int old_num = patternNum;
        patternNum = 0;
        for (auto node: layer) {
            start = find_variation(node, output_patterns, patterns, start, patternNum, minmax);

            if (node->properties->type == Constant) {
                gen_function(node);
            } 
            if (node->left_child() != NULL) {
                gen_function(node->left_child());
                gen_function(node->right_child());
            }

            for (auto child: node->child) {
                next_layer.push_back(child); 
            }
        }
        //fprintf(stderr, "%d start\n", start);
        if (start != old_num) {
            fprintf(stderr, "%d %d\n", start, old_num);
            assert(start == old_num);
        }

        delete[] patterns;
        delete[] output_patterns;
        layer = next_layer;
        fprintf(stderr, "height %d with node %d\n", max_height, this->count);
        max_height++;
    }
    this->care = this->onset.size() < this->offset.size()? ONSET: OFFSET;


}

Forest::Forest(Tree* tree, int pn, SUP sup) {
    MaxTree = tree;
    MinTree.init(pn, sup);
}

void Forest::merge() {
    SUP* sup1 = MaxTree->get_root()->get_support(); 
    CARE set = MaxTree->get_care();
    std::vector<std::vector<int>> *func = MaxTree->get_function(set);
    std::vector<std::vector<int>> *func2 = MinTree.get_function(set);
    for (auto function: *func2) {
        func->push_back(function);
    }
}

void Forest::MiniMax(int height_limit) {
    int max_height_limit = 22;
    int min_height_limit = 20;
    printf("begin on the max tree\n");
    MaxTree->IDAS(max_height_limit, 1);
    printf("begin on the min tree\n");
    MinTree.IDAS(min_height_limit, 0);
    merge();
}

