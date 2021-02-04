#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include <sys/stat.h>
#include <assert.h>
#include <bitset>

#include "io.hpp"
#include "simulate.cpp"
#include "decision.hpp"
#include "blif.cpp"

int PI_N;
int PO_N;
Agent IO; 
std::string caseNum;


#include "pattern.hpp"

extern int PO_N;


void run_abc_opt(char* circuit, char* verilog) {
    std::string command_file = "abc_opt_command.txt"; 
    FILE* fp = fopen("abc_opt_command.txt", "w");
    fprintf(fp, "read %s\n", circuit);
    fprintf(fp, "strash\n");
    fprintf(fp, "rewrite\n");
    fprintf(fp, "dc2\n");
    fprintf(fp, "resyn3\n");
    fprintf(fp, "rewrite\n");
    fprintf(fp, "dc2\n");
    fprintf(fp, "resyn3\n");
    fprintf(fp, "write_verilog %s\n",verilog);
    // fprintf(fp, "sim -A test2.in -v \n");
    fclose(fp);
    std::string command = "./abc <" + command_file + " > output.txt";
    std::system(command.c_str());
}


void read_abc_opt(int PatterNum, Pattern patterns[]) {

    printf("begin to read abc\n");
    char read_file[] = "output.txt";
    FILE* fp = fopen(read_file, "r");
    char* buffer = NULL;
    size_t len;
    if (fp == NULL){
        printf("can not open\n");
        return;
    }
    int read; 
    //char b[1024];
    //fscanf(fp, "%s", b);
    //printf("%s", b);
    for (int i = 0; i < 4+6; i++) {
        getline(&buffer, &len, fp);
    }
    printf("%d\n", PO_N);
    for (int i = 0; i < PatterNum; i++) {
        int read = getline(&buffer, &len, fp);
        if (read != -1) {
            for (int j = 0; j < PO_N; j++) {
                patterns[i].data[j] = buffer[j]=='0'?0:1;
            }
        }
    }

    free(buffer);

    fclose(fp);
     
}


bool executable(const char *file) {
    struct stat st;
    if (stat(file, &st) < 0) return false;
    if (st.st_mode & S_IEXEC) return true;
    return false; 
}

double accurate(int PatterNum, Pattern answer[], Pattern predicts[]) {
    double acc;
    int right = 0;
    for (int i = 0; i < PatterNum; i++) {
        int flag = 1;
        for (int j = 0; j < PO_N; j++) {
            if (answer[i].data[j] != predicts[i].data[j]){
                printf("wrong on output %d\n", j);
                flag = 0; 
            }
        }
        if (flag) {
            right++;
        }
        //printf("\n");
    }
    acc = (double)right/PatterNum;
    return acc; 
}

int main (int argc, char **argv) {

    srand(time(NULL));
    if (argc < 5) {
        fprintf(stderr, "Usage: ./abc_opt <io_info.txt> <iogen> <circuit.blif> <circuit.v> \n");
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
    run_abc_opt(argv[3],argv[4]);
    read_abc_opt(answerNum, predicts);
    double acc = accurate(answerNum, answer_patterns, predicts); 
    printf("------------------------\n");
    printf("acc: %lf\n", acc);

    return 0;
}
