#include <assert.h>
#include <set>
#include <string.h>
#include <time.h>
#include <algorithm>


#include "pattern.hpp"
#include "io.hpp"
#include "support.hpp"

extern Agent IO;
extern int PO_N;
extern int PI_N;

void gen_flip_random(int bit_place, int batchNum, Pattern patterns[]) {
    assert(batchNum%2 == 0); // is 2's multiple
    for (int i = 0; i < batchNum; i+=2) {
        patterns[i].randBitset(); 
        patterns[i+1].data = patterns[i].data;
        patterns[i].data[bit_place].flip();
    }
    return ;
    int pos = batchNum/2 + batchNum/4;
    Pattern tmp;
    tmp.set_size(128);
    for (int i = batchNum/2; i < pos; i+=2) {
        patterns[i].randBitset(); 
        tmp.randBitset();
        patterns[i].data |= tmp.data;
        tmp.randBitset();
        patterns[i].data |= tmp.data;
        tmp.randBitset();
        patterns[i].data |= tmp.data;
        tmp.randBitset();
        patterns[i].data |= tmp.data;
        tmp.randBitset();
        patterns[i].data |= tmp.data;
        patterns[i+1].data = patterns[i].data;
        patterns[i].data[bit_place].flip();
    }
    for (int i = pos; i < batchNum; i+=2) {
        patterns[i].randBitset(); 
        tmp.randBitset();
        patterns[i].data &= tmp.data;
        tmp.randBitset();
        patterns[i].data &= tmp.data;
        tmp.randBitset();
        patterns[i].data &= tmp.data;
        tmp.randBitset();
        patterns[i].data &= tmp.data;
        tmp.randBitset();
        patterns[i].data &= tmp.data;
        patterns[i+1].data = patterns[i].data;
        patterns[i].data[bit_place].flip();
    }
}

void gen_random(int batchNum, Pattern patterns[]) {
    for (int i = 0; i < batchNum; i++) {
        patterns[i].randBitset();
    }
}

void simulate_variation(int input_var, int patternNum, Pattern output_patterns[]) {
    fprintf(stderr,"start finding variation on variable %d\n", input_var); 
    //int batchNum = 200;
    //int patternNum = batchNum;
    Pattern patterns[patternNum];   
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    gen_flip_random(input_var, patternNum, patterns);
    printf("patternnum %d\n", patternNum);
    IO.output_pattern(patternNum, patterns);
    IO.execute();

    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    //Pattern output_patterns[patternNum];
    IO.gen_patterns(patternNum, patterns, output_patterns);
}

void random_variation(int patternNum, Pattern output_patterns[]) {
    fprintf(stderr,"start random testing %d\n", patternNum); 
    Pattern patterns[patternNum];   
    char filename[] = "test2.in";
    FILE* fp = fopen(filename, "w");
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    gen_random(patternNum, patterns);
    IO.output_pattern(patternNum, patterns);
    IO.execute();
    //printf("%d\n", PI_N);
    for (int i = 0; i < patternNum; i++) {
        char pattern[PI_N+1];
        pattern[PI_N] = '\0';
        for (int j = 0; j < PI_N; j++) {
            pattern[j] = patterns[i].data[j]?'1':'0';
        }
        fprintf(fp, "%s\n", pattern);
    }
    fclose(fp);

    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    //Pattern output_patterns[patternNum];
    IO.gen_patterns(patternNum, patterns, output_patterns);
    IO.output_pattern2(patternNum, patterns);
}

/*
void simulate_depend(SUP output, node* parent){

    fprintf(stderr,"start finding\n"); 
    int batchNum = 200;
    int patternNum = batchNum * height;
    Pattern patterns[patternNum];   
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    for (int i = 0; i < height; i++) {
        gen_flip_random(i, batchNum, patterns+batchNum*i);
    }
    IO.output_pattern(patternNum, patterns);
    fprintf(stderr,"begin simulate\n"); 
    clock_t start = clock(); 
    IO.execute();
    clock_t end = clock(); 
    cout << "esclapse time: " << end-start << std::endl;
    
    fprintf(stderr, "end simulate\n");
    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    Pattern output_patterns[patternNum];
    //Pattern patterns2[patternNum];   
    IO.gen_patterns(patternNum, patterns, output_patterns);
    // TODO gen pattern do not need to gen the input patterns !!!
    
    //assert(patterns[0].data == patterns2[0].data);
    int varietyCount[height];
    for (int i = 0; i < height; i++) {
        memset(varietyCount, 0, sizeof(varietyCount));

        Pattern pattern;
        for (int j = 0; j < batchNum; j+=2) {
            pattern.data = output_patterns[j+i*batchNum].data 
                ^ output_patterns[j+i*batchNum+1].data;

            for (int j = 0; j < PO_N; j++) {
                varietyCount[j] += pattern.data[j];
            }
        }
        for (int j = 0; j < PO_N; j++) {
            if (varietyCount[j] != 0) { // output[j] has dependcy on input[i]
                output[j].var.insert(i);
                output[j].piority.push_back(
                        make_pair(i, varietyCount[j]));
            }
        }

    }

    for (int i = 0; i < PO_N; i++) {
        // sort the input variety count 
        output[i].set_idx(i);
        sort(output[i].piority.begin(), output[i].piority.end(), 
                [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                { return  a.second < b.second;});

        output[i].print();
    }
    return ;
}*/

void find_depend(SUP output[]){

    fprintf(stderr,"start finding\n"); 
    int batchNum = 7200;
    //int batchNum = 14400;
    int patternNum = batchNum * PI_N;
    Pattern* patterns = new Pattern[patternNum];   
    // initialize the pattern size
    for (int i = 0; i < patternNum; i++) {
        patterns[i].set_size(PI_N);
    }
    // generate the random patterns 
    for (int i = 0; i < PI_N; i++) {
        gen_flip_random(i, batchNum, patterns+batchNum*i);
    }
    IO.output_pattern(patternNum, patterns);
    fprintf(stderr,"begin simulate\n"); 
    clock_t start = clock(); 
    IO.execute();
    clock_t end = clock(); 
    cout << "esclapse time: " << end-start << std::endl;
    
    fprintf(stderr, "end simulate\n");
    int s_patternNum = IO.read_relation();
    assert(s_patternNum == patternNum);
    Pattern* output_patterns = new Pattern[patternNum];
    //Pattern patterns2[patternNum];   
    IO.gen_patterns(patternNum, patterns, output_patterns);
    // TODO gen pattern do not need to gen the input patterns !!!
    
    //assert(patterns[0].data == patterns2[0].data);
    for (int i = 0; i < PI_N; i++) {
        int varietyCount[PO_N];
        memset(varietyCount, 0, sizeof(varietyCount));

        Pattern pattern;
        for (int j = 0; j < batchNum; j+=2) {
            pattern.data = output_patterns[j+i*batchNum].data 
                ^ output_patterns[j+i*batchNum+1].data;

            for (int j = 0; j < PO_N; j++) {
                varietyCount[j] += pattern.data[j];
            }
        }
        for (int j = 0; j < PO_N; j++) {
            if (varietyCount[j] != 0) { // output[j] has dependcy on input[i]
                output[j].var.insert(i);
                output[j].piority.push_back(
                        make_pair(i, varietyCount[j]));
            }
        }
    }

    for (int i = 0; i < PO_N; i++) {
        // sort the input variety count 
        output[i].set_idx(i);
        sort(output[i].piority.begin(), output[i].piority.end(), 
                [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                { return  a.second < b.second;});

        output[i].print();
        if (output[i].var.size() == 0) {
            output[i].constant = output_patterns[0].data[i]?1:0;
        }
    }
    return ;
}
