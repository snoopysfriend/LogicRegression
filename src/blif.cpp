#include <stdio.h>
#include <string.h>

#include "decision.hpp"
#include "variable.hpp"
#include <assert.h>

extern int PO_N;

void print_to_blif(char* filename, Tree FBDT[], Vars* var) {
    // opening the file
    //char filename[] = "circuit.blif";
    FILE* fp = fopen(filename, "w");
    // gen the names 
    fprintf(fp, ".model LogicRegression\n");
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
        SUP* sups = FBDT[i].get_root()->get_support();
        fprintf(fp, ".names");
        if (sups->var.size() == 0) {
            // constant node
            fprintf(fp, " %s\n", var->outputNames[sups->o_idx].c_str());
            fprintf(fp, "%d\n", sups->constant);
        } else {
            for (auto sup: sups->var) {
                fprintf(fp, " %s", var->inputNames[sup].c_str());
            }
            fprintf(fp, " %s\n", var->outputNames[sups->o_idx].c_str());
            int care = FBDT[i].get_care();  
            if (care == BI) {
                std::vector<std::vector<int>> *func = FBDT[i].get_function(ONSET);
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
                    for (int l = 0; l < input_size; l++) {
                        if (input[l] != '0' && input[l] != '1') {
                            input[l] = '-';
                        }
                    }
                    fprintf(fp, "%s %d\n", input, ONSET);
                }
                func = FBDT[i].get_function(OFFSET);
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
                    for (int l = 0; l < input_size; l++) {
                        if (input[l] != '0' && input[l] != '1') {
                            input[l] = '-';
                        }
                    }
                    fprintf(fp, "%s %d\n", input, OFFSET);
                }
            } else {
                std::vector<std::vector<int>> *func = FBDT[i].get_function();
                int input_size = sups->var.size();
                for (auto function: *func) {
                    char input[input_size+15];
                    memset(input, 0, sizeof(input));
                    input[input_size] = '\0';
                    for (int l = 0; l < input_size; l++) {
                        input[l] = '-';
                    }
                    for (auto lit: function) {
                        int var = lit_to_var(lit);
                        bool sign = lit & 1;
                        auto var_inset = sups->var.find(var);
                        int var_index = std::distance(sups->var.begin(), var_inset); 
                        assert(var_index >= 0 && var_index < input_size);
                        input[var_index] = '0'+ sign;
                    }
                    fprintf(fp, "%s %d\n", input, care);
                }
             }
        }
    }
    fprintf(fp, ".end\n");


    fclose(fp);
}
