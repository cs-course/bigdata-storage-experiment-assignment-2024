#include <iostream>
#include <random>
#include <chrono>
#include "cuckoo2array.hpp"

//g++ .\test1.cpp -o test1 -D__CUCKOO2_DEBUG__
int main() {
    printf("<==== start test of normal cuckoo2array hash ====>\n");
    #ifndef __CUCKOO2_DEBUG__
        printf("use -DCUCKOO2_DEBUG__ to enable debuging info\n");
    #endif
    
    int capacity = 1000000;//997;//
    CUCKOO2<int, int>hash(capacity, 3, 711);// = CUCKOO2<int, int>
    std::default_random_engine generator(998244353);
    std::uniform_int_distribution<int> distribution(1, (int)2e9);
    int i = capacity * 2;
    int fail_cnt = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    while(-- i) {
        int random_num = distribution(generator);
        if (hash.insert(random_num, random_num + 1)) {
            #ifdef __CUCKOO2_DEBUG__
                if (!hash.find(random_num)) {
                    printf("Error\n");
                    return 0;
                }
                // printf("%d %d\n", random_num, ++ hash[random_num]);
            #endif
        }
        else fail_cnt ++;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("<==== end test of normal cuckoo2array hash ====>\n");
    std::cout << "Function took " << duration.count() << " microseconds." << std::endl;
    printf("Try to insert for %d times. Failed %d times.\n", capacity * 2, fail_cnt);
    printf("Usage : %d/%d = %.5lf\%\n", hash.size(), 2 * capacity, 100.0 * hash.size()/(2 * capacity));
    hash.printInfo();
    return 0;
}