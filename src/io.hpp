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

class Agent {
public:

    void IO_INFO(std::string filename) {
        input.open(filename, std::fstream::in); 
        if (!input.is_open()) {
            throw std::runtime_error("Error opening io_info file\n");
        }
        read_info();
        input.close();
    }

    void read_info() {
        input >> inputNum >> outputNum;
        inputNames.resize(inputNum);
        outputNames.resize(outputNum);
        for (int i = 0; i < inputNum; i++) {
            input >> inputNames[i];
        }
        for (int i = 0; i < outputNum; i++) {
            input >> outputNames[i];
        }
    }

    void IO_GEN(std::string exec_name) {
        pid = fork();
        if (!pid) { // child process
            gen_input_pattern(64);
            if (execlp(exec_name.c_str(), exec_name.c_str(), patFilename.c_str(), 
                        relFilename.c_str(), NULL) == -1) {
                fprintf(stderr, "run exec error\n");
                exit(-1);
            }
        }
    }

    void gen_input_pattern(int patternNum) {
        srand(time(NULL));
        std::fstream pattern_io;
        pattern_io.open(patFilename, std::fstream::out);
        pattern_io << inputNum << " " << patternNum << "\n"; 
        for (int i = 0; i < inputNum-1; i++) {
            pattern_io << inputNames[i] << " ";
        }
        pattern_io << inputNames[inputNum-1] << "\n";

        for (int i = 0; i < patternNum; i++) {
            int random = rand(); // random gen the pattern
            std::string pattern;
            for (int j = 0; j < inputNum-1; j++) {
                pattern += (random>>j)&1?"1 ":"0 ";
            }
            pattern += (random>>(inputNum-1))&1?"1\n":"0\n";
            pattern_io << pattern; 
        }
        pattern_io.close();
    }

    void read_relation() {
        std::fstream relation;
        relation.open(relFilename, std::fstream::in);
        int inputNum2; 
        int outputNum2; 
        int patternNum; 
        relation >> inputNum2 >> outputNum2 >> patternNum;
        //std::cout << inputNum2 <<" "<< outputNum2 << std::endl;
        assert(inputNum == inputNum2 && outputNum == outputNum2);
        patterns.resize(patternNum); 
        int size = inputNum2 + outputNum2;
        std::string Names[size];
        for (int i = 0; i < size; i++) {
            relation >> Names[i];
        }
        for (int i = 0; i < patternNum; i++) {
            std::getline(relation, patterns[i]);
            //std::cout << patterns[i] << std::endl;
        }
        relation.close();
    }

private:
    int pid = -1;
    std::fstream input; 
    int inputNum = -1, outputNum = -1;
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    std::string patFilename = "in_pat.txt";
    std::string relFilename = "io_rel.txt";
    std::vector<std::string> patterns;
};

#endif
