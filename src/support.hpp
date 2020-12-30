#ifndef SUPPORT_H
#define SUPPORT_H

#include <set>
#include <vector>
#include <iostream>

class SUP {
public:

    std::set<int> var;
    std::vector<std::pair<int, int>> piority;
    inline int var_num() {
        return var.size();
    }
    void print() {
        std::cout << "output variable depends on " << var.size() << " input variables\n";
        for (auto v: var) {
            std::cout << v << " ";
        }
        std::cout << "\n";
        for (auto p: piority) {
            std::cout << p.first << " " << p.second << "\n";
        }
    }
};

#endif
