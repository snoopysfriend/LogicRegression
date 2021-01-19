#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "pattern.hpp"

extern int PO_N;

void run_abc() {
    std::string command_file = "src/abc_command.txt"; 
    std::string command = "./abc <" + command_file + " > output.txt";
    std::system(command.c_str());
}

void read_abc(int PatterNum, Pattern patterns[]) {

    printf("begin to read abc\n");
    char read_file[] = "output.txt";
    FILE* fp = fopen(read_file, "r");
    char* buffer = NULL;
    size_t len;
    if (fp == NULL)
        return;
    int read; 
    //char b[1024];
    //fscanf(fp, "%s", b);
    //printf("%s", b);
    for (int i = 0; i < 4; i++) {
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


