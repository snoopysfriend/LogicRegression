#ifndef SUPPORT_H
#define SUPPORT_H

#include <set>
#include <vector>
#include <iostream>

class SUP {
public:
    int o_idx;
    std::set<int> var;
    std::vector<std::pair<int, int>> piority; // first the var index second the number 
    SUP() {

    }
    SUP(const SUP& s) {
        o_idx = s.o_idx;
        var = s.var;
        piority = s.piority;
    }

    inline void set_idx(int i) {
        o_idx = i;
    }
    inline int var_num() {
        return var.size();
    }
    void print() {
        std::cout << "output variable " << o_idx << " depends on " << var.size() << " input variables\n";
        for (auto v: var) {
            std::cout << v << " ";
        }
        std::cout << "\n";
        /*
        for (auto p: piority) {
            std::cout << p.first << " " << p.second << "\n";
        }*/
    }
};

#endif
