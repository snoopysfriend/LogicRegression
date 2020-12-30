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
}

void find_depend(SUP output[]){

    fprintf(stderr,"start finding\n"); 
    int batchNum = 100;
    int patternNum = batchNum * PI_N;
    Pattern patterns[patternNum];   
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
    Pattern output_patterns[patternNum];
    Pattern patterns2[patternNum];   
    IO.gen_patterns(patternNum, patterns2, output_patterns);
    //std::cout << patterns[0].data.to_string() << std::endl;
    //std::cout << patterns2[0].data.to_string() << std::endl;
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
        sort(output[i].piority.begin(), output[i].piority.end(), 
                [](std::pair<int, int> const& a, std::pair<int, int> const& b)
                { return  a.second < b.second;});

        output[i].print();
    }
    return ;
}
