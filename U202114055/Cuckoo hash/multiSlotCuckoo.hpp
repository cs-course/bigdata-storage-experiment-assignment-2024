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
        std::uniform_int_distribution<int32_t> distribution01; // 分布对象
        std::uniform_int_distribution<int32_t> distribution_slot; // 分布对象
        std::vector<std::vector<std::pair<K, V>>> hashTable1;
        std::vector<uint8_t> bitset1;
        std::vector<std::vector<std::pair<K, V>>> hashTable2;
        std::vector<uint8_t> bitset2;

        int32_t my_hash1(const K &key) const {
            return std::hash<K>{}(key) % capacity;
        }

        int32_t my_hash2(const K &key) const {
            int32_t h = std::hash<K>{}(key);
            return (h ^ (0x9e3779b9 + (1ll * h << 6) + (h >> 2))) % capacity;
        }
        
    public:
        multi_slot_cuckoo(int32_t capacity, int32_t max_depth = 10, int32_t slot_num = 4, uint32_t seedValue = 0) 
                    : capacity(capacity), max_depth(max_depth), seed(seedValue), Size(0), bitset1(capacity), bitset2(capacity) {
            if (!seed) {
                // 如果种子为0，则使用当前时间作为种子
                seed = static_cast<uint32_t>(std::time(nullptr));
            } 
            generator.seed(seed); // 初始化随机数生成器
            distribution01 = std::uniform_int_distribution<int>(0, 1); // 初始化分布对象
            
            if (slot_num <= 1) slot_num = 2;
            if (slot_num > 8) slot_num = 8;
            this->slot_num = slot_num;

            distribution_slot = std::uniform_int_distribution<int>(0, slot_num);
            
            hashTable1.resize(capacity);
            hashTable2.resize(capacity);
            for (std::vector<std::pair<K, V>>& col : hashTable1) {
                col.resize(slot_num);
            }
            for (std::vector<std::pair<K, V>>& col : hashTable2) {
                col.resize(slot_num);
            }
        }
        /*
        @param key 需要插入的数据的索引
        @param data 需要存储的数据
        @return 是否插入成功 1成功，0失败
        */
        bool insert(const K &key, const V &data) {

        }
        /*
        @param key 需要移除的数据的索引
        */
        void remove(const K &key) {

        }
        /*
        @param key 需要查找是否存在的索引
        @return 1表示存在，0不存在
        */
        bool find(const K &key)  {
            int32_t index1 = my_hash1(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset1[index] & (1 << i) && hashTable1[index][i].first == key) {
                    return true;
                }
            }
            int32_t index2 = my_hash2(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset2[index] & (1 << i) && hashTable2[index][i].first == key) {
                    return true;
                }
            }
            return false;
        }

        inline uint32_t size() {
            return Size;
        }
        inline uint32_t get_seed() {
            return seed;
        }
};

#endif