#include <stdio.h>
#include <string>
#include <getopt.h>
#include <sys/stat.h>
#include <assert.h>
#include <bitset>

#include "io.hpp"
#include "simulate.cpp"
#include "decision.hpp"
#include "blif.cpp"
#include "abc.cpp"

int PI_N;
int PO_N;
Agent IO; 
std::string caseNum;

bool executable(const char *file) {
    struct stat st;
    if (stat(file, &st) < 0) return false;
    if (st.st_mode & S_IEXEC) return true;
    return false; 
}

double accurate(int PatterNum, Pattern answer[], Pattern predicts[]) {
    double acc;
    int right = 0;
    int pos = 0;
    int neg = 0;
    //int zero = 0;
    //int one = 0;
    for (int i = 0; i < PatterNum; i++) {
        int flag = 1;
        for (int j = 0; j < PO_N; j++) {
            if (answer[i].data[j] != predicts[i].data[j]){
                printf("%d wrong on output %d \n", i, j);
                flag = 0; 
                /*if (j == 4) {
                    if (answer[i].data[j] == 1) {
                        pos++;
                    } else {
                        neg++;
                    }
                }*/
            }
            /*if (j == 4) {
                if (answer[i].data[j] == 1) {
                    one++;
                } else {
                    zero++;
                }
            }*/
        }
        if (flag) {
            right++;
        }
    }
    //printf("false pos %d %d\n", pos, one);
    //printf("false neg %d %d\n", neg, zero);
    acc = (double)right/PatterNum;
    return acc; 
}

int main (int argc, char **argv) {

    srand(time(NULL));
    if (argc < 4) {
        fprintf(stderr, "Usage: ./test <io_info.txt> <iogen> <circuit.v> \n");
        exit(-1);
    }

    std::string ioinfo;
    std::string iogen;
    std::string outputCircuit;

    ioinfo = argv[1];
    iogen = argv[2];
    outputCircuit = argv[3];

    if (!executable(iogen.c_str())) {
        fprintf(stderr, "The input file is not executable\n");
    }

    Vars* vars = new Vars;

    IO.set_vars(vars);
    IO.set_executename(iogen);

    IO.IO_INFO(ioinfo); // read io info
    std::cout << "PI_N: " << PI_N << std::endl;
    std::cout << "PO_N: " << PO_N << std::endl;
    int patternNum = 100000; 
    Pattern test_patterns[patternNum];
    random_variation(patternNum, test_patterns);
    int answerNum = IO.read_relation();
    Pattern answer_patterns[answerNum];
    Pattern predicts[answerNum];
    assert(answerNum == patternNum);
    IO.gen_patterns(patternNum, test_patterns, answer_patterns);
    printf("begin to run abc\n");
    run_abc(argv[3]);
    read_abc(answerNum, predicts);
    for (int i = 0; i < PO_N; i++){
        printf("%c", predicts[0].data[i]==1?'1':'0');
    }
    printf("\n");
    for (int i = 0; i < PO_N; i++){
        printf("%c", answer_patterns[0].data[i]==1?'1':'0');
    }
    printf("\n");
    double acc = accurate(answerNum, answer_patterns, predicts); 
    printf("------------------------\n");
    printf("acc: %lf\n", acc);

    return 0;
}
