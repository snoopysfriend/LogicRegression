#include <stdio.h>
#include <string>
#include <getopt.h>
#include <sys/stat.h>
#include <assert.h>

#include "io.hpp"
#include "simulate.cpp"

//  
extern int PI_N;
extern int PO_N;
extern Agent IO; 

bool executable(const char *file) {
    struct stat st;
    if (stat(file, &st) < 0) return false;
    if (st.st_mode & S_IEXEC) return true;
    return false; 
}

int main (int argc, char **argv) {

    if (argc < 4) {
        fprintf(stderr, "Usage: ./lrg <io_info.txt> <iogen> <circuit.v>\n");
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
    SUP output[PO_N];
    find_depend(output);


    return 0;
}
