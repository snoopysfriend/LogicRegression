#ifndef IO_HPP
#define IO_HPP
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <assert.h>
#include <bitset>
#include <bits/stdc++.h>

#include "variable.hpp"
#include "pattern.hpp"


extern int PI_N;
extern int PO_N;

class Agent {
public:
    Agent() {

    }

    void set_vars(Vars* v) {
        if (v == NULL) {
            throw std::runtime_error("variables should not be null\n");
        }
        vars = v;
    }
    
    
    void IO_INFO(std::string filename) {
        input.open(filename, std::fstream::in); 
        if (!input.is_open()) {
            throw std::runtime_error("Error opening io_info file\n");
        }
        read_info();
        PI_N = vars->inputNum;
        PO_N = vars->outputNum;
        input.close();
    }

    void read_info() {
        printf("reading\n");
        input >> vars->inputNum >> vars->outputNum;
        vars->inputNames.resize(vars->inputNum);
        vars->outputNames.resize(vars->outputNum);
        for (int i = 0; i < vars->inputNum; i++) {
            input >> vars->inputNames[i];
        }
        for (int i = 0; i < vars->outputNum; i++) {
            input >> vars->outputNames[i];
        }
        printf("reading end\n");
    }

    void IO_GEN(std::string exec_name, int patternNum, Pattern patterns[]) { 
        pid = fork();
        if (!pid) { // child process
            output_pattern(patternNum, patterns);
            if (execlp(exec_name.c_str(), exec_name.c_str(), patFilename.c_str(), 
                        relFilename.c_str(), NULL) == -1) {
                fprintf(stderr, "run exec error\n");
                exit(-1);
            }
        }
    }

    void set_executename(std::string name) {
        exec_name = name;
    }

    void execute() {
        string command = exec_name + " " + patFilename + " "+ relFilename; 
        std::system(command.c_str());
    }

    void output_pattern(int patternNum, Pattern patterns[]) {
        //srand(time(NULL));
        std::fstream pattern_io;
        pattern_io.open(patFilename, std::fstream::out);
        pattern_io << vars->inputNum << " " << patternNum << "\n"; 
        for (int i = 0; i < vars->inputNum-1; i++) {
            pattern_io << vars->inputNames[i] << " ";
        }
        pattern_io << vars->inputNames[vars->inputNum-1] << "\n";

        for (int i = 0; i < patternNum; i++) {
            std::string pattern;
            pattern += patterns[i].data[0]?"1":"0";
            for (int j = 1; j < vars->inputNum; j++){
                pattern += patterns[i].data[j]?" 1":" 0";
            }
            pattern += "\n";
            pattern_io << pattern; 
        }
        pattern_io.close();
    }

    int read_relation() {
        std::fstream relation;
        relation.open(relFilename, std::fstream::in);
        int inputNum2; 
        int outputNum2; 
        int patternNum; 
        relation >> inputNum2 >> outputNum2 >> patternNum ;
        //assert(inputNum == inputNum2 && outputNum == outputNum2);
        patterns_s.resize(patternNum); 
        int size = inputNum2 + outputNum2;
        std::string Names[size];
        std::getline(relation, Names[0]);
        std::getline(relation, Names[1]);
        /*
        for (int i = 0; i < size; i++) {
            relation >> Names[i];
        }*/
        for (int i = 0; i < patternNum; i++) {
            std::getline(relation, patterns_s[i]);
            //std::cout << patterns_s[i] << std::endl;
        }
        sleep(1);
        relation.close();
        return patternNum;
    }

    void gen_patterns(int patternNum, Pattern patterns[], Pattern output_patterns[]) {
        // TODO do we need to get the input patterns?
        for (int i = 0; i < patternNum; i++) {
            //cout << patterns_s[i] << std::endl;
            for (int j = 0; j < PI_N; j++) {
                //cout << ((patterns_s[i][j<<1] =='1')?'1':'0');
                patterns[i].data[j] = ((patterns_s[i][j<<1]=='1')?1:0); 
            }
            //cout << std::endl;
            //std::cout << patterns[i].data.to_string() << std::endl;
            output_patterns[i].set_size(PO_N);
            for (int j = 0; j < PO_N; j++) {
                output_patterns[i].data[j] = (patterns_s[i][(j+PI_N)<<1]=='1'?1:0); 
            }
        }

    }

    inline int PiNum() {
        return vars->inputNum;
    }

    inline int PoNum() {
        return vars->outputNum;
    }

private:
    int pid = -1;
    std::fstream input; 
    Vars* vars;
    std::string exec_name;
    std::string patFilename = "in_pat.txt";
    std::string relFilename = "io_rel.txt";
    std::vector<std::string> patterns_s;
};

#endif
