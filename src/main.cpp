#include <stdio.h>
#include <string>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h> 
#include <assert.h>

#include "io.hpp"
#include "simulate.cpp"
#include "decision.hpp"
#include "blif.cpp"


int PI_N;
int PO_N;
Agent IO; 
std::string caseNum_s;

int sec = 3600;

bool executable(const char *file) {
    struct stat st;
    if (stat(file, &st) < 0) return false;
    if (st.st_mode & S_IEXEC) return true;
    return false; 
}

void sigroutine(int signo) {
    switch (signo) {
        case SIGALRM:
            printf("Catch a signal timeout\n");
            exit(0);
    }
    return ;
}

int main (int argc, char **argv) {

    srand(time(NULL));
    if (argc < 4) {
        fprintf(stderr, "Usage: ./lrg <io_info.txt> <iogen> <circuit.v>\n");
        exit(-1);
    }
    struct itimerval value, ovalue;
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = sec;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = sec;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);

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

    SUP output[PO_N]; 
    find_depend(output); // find the output dependency variable


    Tree FDBTS[PO_N];
    int height_limit = 30;
#ifdef SFLIP
    for (int i = 0; i < PO_N; i++) {
        FDBTS[i].init(PI_N, output[i]);
        if (i >= PO_N ) {
            continue;
        }
        if (output[i].var.size() > 0) {
            fprintf(stderr, "output %d var size %d\n", i, output[i].var.size());
            if (output[i].var.size() > 18) {
                FDBTS[i].SPFLIP(FDBTS[i].get_root());
                //FDBTS[i].sp_flip(FDBTS[i].get_root());
            } else {
                SUP* sup = FDBTS[i].get_root()->get_support();
                sup->print();
                FDBTS[i].brute_force(sup);
            }
        }
        else {
            FDBTS[i].init(PI_N, output[i]);
            printf("constant node %d\n", output[i].var.size());
        }
    }
    print_to_blif(argv[3], FDBTS, vars);
#else
    for (int i = 0; i < PO_N; i++) {
        if (output[i].var.size() > 0) {
            fprintf(stderr, "var size %d\n", output[i].var.size());
            FDBTS[i].init(PI_N, output[i]);
            if (output[i].var.size() < 18) {
                SUP* sup = FDBTS[i].get_root()->get_support();
                FDBTS[i].brute_force(sup);
                FDBTS[i].print();
            } else {
                printf("var too many %d\n", output[i].var.size());
                printf("Begin using unate paradim\n");
                if (output[i].var.size() >  30) {
                    fprintf(stderr, "large tree!!! We need a forest\n");
                    Forest forest(&FDBTS[i], PI_N, output[i]);
                    forest.MiniMax(20);
                    forest.merge();
                    //FDBTS[i].IDAS(height_limit);
                    //FDBTS[i].IDAS(height_limit, true);
                } else {
                    Forest forest(&FDBTS[i], PI_N, output[i]);
                    forest.MiniMax(20);
                    forest.merge();
                    //FDBTS[i].IDAS(height_limit, true);
                    //FDBTS[i].unate_paradim(height_limit, 1);
                }
            }
        } else {
            FDBTS[i].init(PI_N, output[i]);
            printf("constant node %d\n", output[i].var.size());
        }
    }
    print_to_blif(argv[3], FDBTS, vars);

#endif


    return 0;
}
