#include <iostream>
#include <random>
#include "cuckoo2array.hpp"

int main() {
    printf("====start test of normal cuckoo2array hash====\n");
    #ifndef __CUCKOO2_DEBUG__
        printf("use -DCUCKOO2_DEBUG__ to enable debuging info\n");
    #endif
    int capacity = 997;
    CUCKOO2<int, int>hash(capacity, 5, 711);// = CUCKOO2<int, int>
    std::default_random_engine generator(998244353);
    std::uniform_int_distribution<int> distribution(1, (int)2e9);
    int i = capacity * 2.5;
    int fail_cnt = 0;
    while(-- i) {
        int random_num = distribution(generator);
        if (hash.insert(random_num, random_num + 1)) {
            #ifdef __CUCKOO2_DEBUG__
                if (!hash.find(random_num)) {
                    printf("Error\n");
                    return 0;
                }
                printf("%d %d\n", random_num, hash[random_num]);
            #endif
        }
    }
    printf("====end test of normal cuckoo2array hash====\n");
    printf("Usage : %d/%d = %.5lf\%", hash.size(), 2 * capacity, 100.0 * hash.size()/(2 * capacity));
    return 0;
}