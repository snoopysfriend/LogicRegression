#include <stdio.h>
#include <string>
#include <getopt.h>
#include <sys/stat.h>
#include <assert.h>

#include "io.hpp"
#include "depend.cpp"

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
    Agent IO(vars);
    IO.IO_INFO(ioinfo);
    // may be need to set the gen random pattern as function
    Pattern patterns[64];
    for (int i = 0; i < 64; i++) {
        patterns[i].set_size(vars->inputNum);
        patterns[i].randBitset();
    }
    IO.IO_GEN(iogen, 64, patterns);

    sleep(1);
    IO.read_relation();


    return 0;
}
