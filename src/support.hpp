#ifndef SUPPORT_H
#define SUPPORT_H

#include <set>
#include <vector>
#include <iostream>

class SUP {
public:

    std::set<int> var;
    std::vector<int> piority;

    void print() {
        std::cout << "output variable depends on " << var.size() << " input variables\n";
        for (auto v: var) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
};

#endif
