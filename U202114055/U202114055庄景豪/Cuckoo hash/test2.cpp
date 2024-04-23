#include <iostream>
#include <random>
#include <chrono>
#include "multiSlotCuckoo.hpp"

//g++ .\test2.cpp -o test2 -D__CUCKOO2_DEBUG__
int main() {
    printf("<==== start test of multi-slot cuckoo2array hash ====>\n");
    #ifndef __CUCKOO2_DEBUG__
        printf("use -DCUCKOO2_DEBUG__ to enable debuging info\n");
    #endif
    int slot_num = 8;//4;//2;//
    int capacity = 12500;//25000;//50000;//
    multi_slot_cuckoo<int, int>hash(capacity, 3, slot_num, 711);// = CUCKOO2<int, int>
    std::default_random_engine generator(998244353);
    std::uniform_int_distribution<int> distribution(1, (int)2e9);
    int i = capacity * slot_num * 2;
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
    printf("<==== end test of multi-slot cuckoo2array hash ====>\n");
    std::cout << "Function took " << duration.count() << " microseconds." << std::endl;
    printf("Try to insert for %d times. Failed %d times.\n", 2 * slot_num * capacity, fail_cnt);
    printf("Usage : %d/%d = %.5lf\%\n", hash.size(), 2 * slot_num * capacity, 100.0 * hash.size()/(2 * slot_num * capacity));
    hash.printInfo();
    return 0;
}