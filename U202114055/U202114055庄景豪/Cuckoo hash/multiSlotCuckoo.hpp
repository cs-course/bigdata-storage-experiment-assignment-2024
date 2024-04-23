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
        uint32_t seize_times = 0, total_depth = 0; // for test
        std::default_random_engine generator; // 随机数生成器
        std::uniform_int_distribution<int32_t> distribution01; // 分布对象
        std::vector<std::vector<std::pair<K, V>>> hashTable1;
        std::vector<uint8_t> bitset1;
        std::vector<std::vector<std::pair<K, V>>> hashTable2;
        std::vector<uint8_t> bitset2;

        int32_t my_hash1(const K &key) const {
            return std::hash<K>{}(key) % capacity;
        }

        int32_t my_hash2(const K &key) const {
            int32_t h = std::hash<K>{}(key);
            return (h ^ (0x9e3779b9 + (1ll * h << 6) + (h >> 2))) % capacity; // 为了避免hash1和hash2不独立
        }

        int32_t find_slot(uint8_t status) const {
            int32_t ret = 0;
            while(status & 1) {
                status >>= 1;
                ret ++;
            }
            if (ret >= slot_num) { // 按理来说不会触发，因为先特判了status不等于full
                printf("No free Slot");
                ret = -1;
            }
            return ret;
        }
        /*
        @param tableIndex 目前所在的表
        @param index 目前需要移到备用位置的对象的index
        @param depth 已经找了几层了
        @return 在第几层找到的（移动了几个原有的）
        */
        int32_t seize_place(int8_t tableIndex, int32_t index, int32_t depth) {
            
            if (depth > max_depth) {
                return 0;
            }
            const uint8_t full = (1 << slot_num) - 1;
            auto my_hash = (tableIndex ^ 1) ? &multi_slot_cuckoo::my_hash2 : &multi_slot_cuckoo::my_hash1;
            std::vector<uint8_t>& bitset = tableIndex ? bitset2 : bitset1;
            std::vector<uint8_t>& nextBitset = (tableIndex ^ 1) ? bitset2 : bitset1;
            std::vector<std::vector<std::pair<K, V>>>& hashTable = tableIndex ? hashTable2 : hashTable1;
            std::vector<std::vector<std::pair<K, V>>>& nextHashTable = (tableIndex ^ 1) ? hashTable2 : hashTable1;
            
            for(int32_t i = 0;i < slot_num;++ i) {
                K& now_key = hashTable[index][i].first; 
                // printf("%d %d %d %d %d %d\n", tableIndex, index, depth, now_key, my_hash1(now_key), my_hash2(now_key));
                int32_t nextIndex = (this->*my_hash)(now_key);
                int32_t ret;
                if (nextBitset[nextIndex] != full) {
                    // 这个的下一个有空
                    ret = depth;
                }
                else if (!(ret = seize_place(tableIndex ^ 1, nextIndex, depth + 1))) {
                    // 这个的下一个没找到
                    continue;
                }
                int32_t free_slot = find_slot(nextBitset[nextIndex]);
                nextBitset[nextIndex] |= (1 << free_slot);
                nextHashTable[nextIndex][free_slot] = hashTable[index][i];
                bitset[index] -= (1 << i);
                return ret;
            }
            
            return 0;
        }
        
    public:
        multi_slot_cuckoo(int32_t capacity, int32_t max_depth = 3, int32_t slot_num = 4, uint32_t seedValue = 0) 
                    : capacity(capacity), max_depth(max_depth), seed(seedValue), Size(0), bitset1(capacity), bitset2(capacity) {
            if (!seed) {
                // 如果种子为0，则使用当前时间作为种子
                seed = static_cast<uint32_t>(std::time(nullptr));
            } 
            generator.seed(seed); // 初始化随机数生成器
            distribution01 = std::uniform_int_distribution<int>(0, 1); // 初始化分布对象
            
            if (max_depth > 3) {
                this->max_depth = 3; // 8 ^ 3 = 512
            }

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
        /*
        @param key 需要插入的数据的索引
        @param data 需要存储的数据
        @return 是否插入成功 1成功，0失败
        */
        bool insert(const K &key, const V &data) {
            int32_t index1 = my_hash1(key);
            int32_t index2 = my_hash2(key);
            const uint8_t full = (1 << slot_num) - 1;
            bool free1 = (bitset1[index1] != full);
            bool free2 = (bitset2[index2] != full);
            bool random_num = distribution01(generator);
            if (free1 && !free2) {
                random_num = 0;
            }
            if (free2 && !free1) {
                random_num = 1;
            }
            int32_t index = random_num ? index2 : index1;
            std::vector<uint8_t>& bitset = random_num ? bitset2 : bitset1;
            std::vector<std::vector<std::pair<K, V>>>& hashTable = random_num ? hashTable2 : hashTable1;
            
            if (!free1 && !free2) {
                int32_t depth = 0;
                seize_times ++;
                if (!(depth = seize_place(random_num, index, 1))) {
                    total_depth += max_depth; // 失败肯定搜满了

                    random_num ^= 1; // 随机的插入遇到死循环了，试试另一边
                    int32_t index = random_num ? index2 : index1;
                    std::vector<uint8_t>& bitset = random_num ? bitset2 : bitset1;
                    std::vector<std::vector<std::pair<K, V>>>& hashTable = random_num ? hashTable2 : hashTable1;
                    if (!(depth = seize_place(random_num, index, 1))) {
                        total_depth += max_depth;
                        // 都不行，报错
                        // expand_capacity();
                        #ifdef __CUCKOO2_DEBUG__
                            printf("Failed to seize place.\n");
                        #endif
                        return false;
                    }
                    #ifdef __CUCKOO2_DEBUG__
                        printf("Seized place for %d cycles.\n", depth);
                    #endif
                    total_depth += depth;
                    int32_t slot_index = find_slot(bitset[index]);
                    bitset[index] |= (1 << slot_index);
                    hashTable[index][slot_index] = std::pair<K, V>(key, data);
                    Size ++;
                    return true; // 因为上面改了引用所以要单独处理                    
                }
                total_depth += depth;
                #ifdef __CUCKOO2_DEBUG__
                    printf("Seized place for %d cycles.\n", depth);
                #endif
            }
            int32_t slot_index = find_slot(bitset[index]);
            bitset[index] |= (1 << slot_index);
            hashTable[index][slot_index] = std::pair<K, V>(key, data);
            Size ++;
            return true;
        }
        /*
        @param key 需要移除的数据的索引
        */
        void remove(const K &key) {
            int32_t index1 = my_hash1(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset1[index1] & (1 << i) && hashTable1[index1][i].first == key) {
                    bitset1[index1] - (1 << i);
                    Size --;
                    return;
                }
            }
            int32_t index2 = my_hash2(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset2[index2] & (1 << i) && hashTable2[index2][i].first == key) {
                    bitset2[index2] - (1 << i);
                    Size --;
                    return;
                }
            }
        }
        /*
        @param key 需要查找是否存在的索引
        @return 1表示存在，0不存在
        */
        bool find(const K &key) const {
            int32_t index1 = my_hash1(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset1[index1] & (1 << i) && hashTable1[index1][i].first == key) {
                    return true;
                }
            }
            int32_t index2 = my_hash2(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset2[index2] & (1 << i) && hashTable2[index2][i].first == key) {
                    return true;
                }
            }
            return false;
        }

        V& operator[](const K &key) {
            int32_t index1 = my_hash1(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset1[index1] & (1 << i) && hashTable1[index1][i].first == key) {
                    return std::get<1>(hashTable1[index1][i]); // 返回second的引用
                }
            }
            int32_t index2 = my_hash2(key);
            for(int32_t i = 0;i < slot_num;++ i) {
                if (bitset2[index2] & (1 << i) && hashTable2[index2][i].first == key) {
                    return std::get<1>(hashTable2[index2][i]);
                }
            }
            // 没找到，抛出异常
            throw std::out_of_range("Key not found");
        }

        inline uint32_t size() const {
            return Size;
        }

        inline uint32_t get_seed() const {
            return seed;
        }
        // 因为有的要两边找，所以平均大于max_depth是正常的
        void printInfo() const {
            if (seize_times)
            printf("Seized for %d times, average serached for %lf layers.\n", seize_times, 1.0 * total_depth / seize_times);
        }
};

#endif