#ifndef __MULTISLOTCUCKOO_H__
#define __MULTOSLOTCUCKOO_H__

#include <stdio.h>
#include <ctime>
#include <random>
#include <vector>
#include <functional>

template<typename K, typename V>
class multi_slot_cuckoo {
    private:
        int32_t capacity;
        int32_t max_depth;
        int32_t slot_num;
        uint32_t Size;
        uint32_t seed;
        std::default_random_engine generator; // 随机数生成器
        std::uniform_int_distribution<int32_t> distribution; // 分布对象
        std::vector<std::vector<std::pair<K, V>>> hashTable1;
        std::vector<uint8_t> bitset1;
        std::vector<std::vector<std::pair<K, V>>> hashTable2;
        std::vector<uint8_t> bitset2;
    public:
        multi_slot_cuckoo(int32_t capacity, int32_t max_depth = 10, int32_t slot_num = 4, uint32_t seedValue = 0) 
                    : capacity(capacity), max_depth(max_depth), seed(seedValue), Size(0), bitset1(capacity), bitset2(capacity) {
            if (!seed) {
                // 如果种子为0，则使用当前时间作为种子
                seed = static_cast<uint32_t>(std::time(nullptr));
            } 
            generator.seed(seed); // 初始化随机数生成器
            distribution = std::uniform_int_distribution<int>(0, 1); // 初始化分布对象

            if (slot_num <= 1) slot_num = 2;
            if (slot_num > 8) slot_num = 8;
            this->slot_num = slot_num;
            hashTable1.resize(capacity);
            hashTable2.resize(capacity);
            for (std::vector<std::pair<K, V>>& col : hashTable1) {
                col.resize(slot_num);
            }
            for (std::vector<std::pair<K, V>>& col : hashTable2) {
                col.resize(slot_num);
            }
        }
        
};

#endif