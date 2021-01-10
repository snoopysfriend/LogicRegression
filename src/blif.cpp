#include <stdio.h>

#include "decision.hpp"
#include "variable.hpp"

void print_to_blif(Tree FBDT[], Vars* var) {

    // opening the file
    char filename[] = "circuit.blif";
    FILE* fp = fopen(filename, "w");
    // gen the names 
    fprintf(fp, ".models LogicRegression\n");
    fprintf(fp, ".inputs");
    for (auto inputName: var->inputNames) {
        fprintf(fp, " %s", inputName.c_str());
    }
    fprintf(fp, "\n.outputs ");
    for (auto outputName: var->outputNames) {
        fprintf(fp, " %s", outputName.c_str());
    }
    fprintf(fp, "\n");

    int o_len = var->outputNum;
    for (int i = 0; i < o_len; i++) {
        fprintf(fp, ".names");
        SUP* sups = FBDT[i].get_root()->get_support();
        for (auto sup: sups->var) {
            fprintf(fp, " %s", var->inputNames[sup].c_str());
        }
        fprintf(fp, " %s\n", var->outputNames[sups->o_idx].c_str());
        int care = FBDT[i].get_care();  
        std::vector<std::vector<int>> *func = FBDT[i].get_function();
        int input_size = sups->var.size();
        for (auto function: *func) {
            char input[input_size+1];
            input[input_size] = '\0';
            for (auto lit: function) {
                int var = lit_to_var(lit);
                bool sign = lit & 1;
                auto var_inset = sups->var.find(var);
                int var_index = std::distance(sups->var.begin(), var_inset); 
                input[var_index] = '0'+ sign;
            }
            fprintf(fp, "%s %d\n", input, care);
        }
        fprintf(fp, ".end\n");
    }


    fclose(fp);
}
