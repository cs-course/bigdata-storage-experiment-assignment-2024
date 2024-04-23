#ifndef __CUCKOO2ARRAY_HPP__
#define __CUCKOO2ARRAY_HPP__

#include <stdio.h>
#include <ctime> // for std::time
#include <vector>
#include <random>
#include <stdexcept>
#include <functional> // for std::hash

template<typename K, typename V>
class CUCKOO2 {
    private:
        int32_t capacity;
        int32_t max_depth;
        uint32_t Size;
        uint32_t seed;
        uint32_t seize_times = 0, total_depth = 0; // for test
        std::default_random_engine generator; // 随机数生成器
        std::uniform_int_distribution<int32_t> distribution; // 分布对象
        std::vector<std::pair<K, V>> hashTable1;
        std::vector<int8_t> bitset1; // bitset 不能动态设置大小，vector<bool>有缺陷
        std::vector<std::pair<K, V>> hashTable2;
        std::vector<int8_t> bitset2;

        int32_t my_hash1(const K &key) const {
            return std::hash<K>{}(key) % capacity;
        }

        int32_t my_hash2(const K &key) const {
            int32_t h = std::hash<K>{}(key);
            return (h ^ (0x9e3779b9 + (1ll * h << 6) + (h >> 2))) % capacity;
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
            auto my_hash = (tableIndex ^ 1) ? &CUCKOO2::my_hash2 : &CUCKOO2::my_hash1;
            std::vector<int8_t>& nextBitset = (tableIndex ^ 1) ? bitset2 : bitset1;
            std::vector<std::pair<K,V>>& hashTable = tableIndex ? hashTable2 : hashTable1;
            std::vector<std::pair<K,V>>& nextHashTable = (tableIndex ^ 1) ? hashTable2 : hashTable1;
            K& now_key = hashTable[index].first; 
            // printf("%d %d %d %d %d %d\n", tableIndex, index, depth, now_key, my_hash1(now_key), my_hash2(now_key));
            int32_t nextIndex = (this->*my_hash)(now_key);
            int32_t ret;
            if (!nextBitset[nextIndex]) {
                ret = depth;
            }
            else if (!(ret = seize_place(tableIndex ^ 1, nextIndex, depth + 1))) {
                return 0;
            }
            nextBitset[nextIndex] = true;
            nextHashTable[nextIndex] = hashTable[index];
            return ret;
        }
        // void expand_capacity() { //扩容之后 capacity 就不是质数了，碰撞概率大大增加
        //     int32_t preCapacity = capacity;
        //     capacity <<= 1;
        //     std::vector<std::pair<K, V>> preHashTable1(capacity);
        //     std::vector<int8_t> preBitset1(capacity); // bitset 不能动态设置大小，vector<bool>有缺陷
        //     std::vector<std::pair<K, V>> preHashTable2(capacity);
        //     std::vector<int8_t> preBitset2(capacity);
        //     hashTable1.swap(preHashTable1);
        //     bitset1.swap(preBitset1);
        //     hashTable2.swap(preHashTable2);
        //     bitset2.swap(preBitset2);

        //     for(int32_t i = 0; i < preCapacity;++ i) {
        //         if (preBitset1[i]) {
        //             insert()
        //         }
        //         if (preBitset2[i]) {
        //             insert()
        //         }
        //     }
        // }
    public:
        /*
        @param capacity 设置hashMap的容量
        @param max_depth 最大搜索深度
        @param seedValue 随机种子
        */
        CUCKOO2(int32_t capacity, int32_t max_depth = 10, uint32_t seedValue = 0)
                        : capacity(capacity), max_depth(max_depth), Size(0)
                        , bitset1(capacity), bitset2(capacity), seed(seedValue) {
            if (!seed) {
                // 如果种子为0，则使用当前时间作为种子
                seed = static_cast<uint32_t>(std::time(nullptr));
            } 
            generator.seed(seed); // 初始化随机数生成器
            distribution = std::uniform_int_distribution<int>(0, 1); // 初始化分布对象
            hashTable1.resize(capacity);
            hashTable2.resize(capacity);
        }
        /*
        @param key 需要插入的数据的索引
        @param data 需要存储的数据
        @return 是否插入成功 1成功，0失败
        */
        bool insert(const K &key, const V &data) {
            int32_t index1 = my_hash1(key);
            int32_t index2 = my_hash2(key);
            bool free1 = !bitset1[index1];
            bool free2 = !bitset2[index2];
            bool random_num = distribution(generator); // 随机找一个插入
            if (free1 && !free2) {
                random_num = 0;
            }
            if (free2 && !free1) {
                random_num = 1;
            }
            int32_t index = random_num ? index2 : index1;
            std::vector<int8_t>& bitset = random_num ? bitset2 : bitset1;
            std::vector<std::pair<K,V>>& hashTable = random_num ? hashTable2 : hashTable1;

            if (!free1 && !free2) {
                int32_t depth = 0;
                seize_times ++;
                if (!(depth = seize_place(random_num, index, 1))) {
                    total_depth += max_depth; // 失败肯定搜满了

                    random_num ^= 1; // 随机的插入遇到死循环了，试试另一边
                    int32_t index = random_num ? index2 : index1;
                    std::vector<int8_t>& bitset = random_num ? bitset2 : bitset1;
                    std::vector<std::pair<K,V>>& hashTable = random_num ? hashTable2 : hashTable1;
                    if (!(depth = seize_place(random_num, index, 1))) {
                        total_depth += max_depth;
                        // 都不行，报错
                        // expand_capacity();
                        #ifdef __CUCKOO2_DEBUG__
                            //printf("Failed to seize place.\n");
                        #endif
                        return false;
                    }
                    #ifdef __CUCKOO2_DEBUG__
                        printf("Seized place for %d cycles.\n", depth);
                    #endif
                    total_depth += depth;
                    bitset[index] = true; // 这里换了引用
                    hashTable[index] = std::pair<K, V>(key, data);
                    Size ++;
                    return true;
                }
                total_depth += depth;
                #ifdef __CUCKOO2_DEBUG__
                    printf("Seized place for %d cycles.\n", depth);
                #endif
            }

            bitset[index] = true;
            hashTable[index] = std::pair<K, V>(key, data);
            Size ++;
            return true;
        }
        /*
        @param key 需要移除的数据的索引
        */
        void remove(const K &key) {
            int32_t index1 = my_hash1(key);
            if (bitset1[index1] && hashTable1[index1].first == key) {
                bitset1[index1] = 0;
                Size --;
                return;
            }
            int32_t index2 = my_hash2(key);
            if (bitset2[index2] && hashTable2[index2].first == key) {
                bitset2[index2] = 0;
                Size --;
                return;
            }
        }
        /*
        @param key 需要查找是否存在的索引
        @return 1表示存在，0不存在
        */
        bool find(const K &key) const {
            int32_t index1 = my_hash1(key);
            if (bitset1[index1] && hashTable1[index1].first == key) {
                return true;
            }
            int32_t index2 = my_hash2(key);
            if (bitset2[index2] && hashTable2[index2].first == key) {
                return true;
            }
            return false;
        }

        V& operator[](const K &key) {
            int32_t index1 = my_hash1(key);
            if (bitset1[index1] && hashTable1[index1].first == key) {
                return std::get<1>(hashTable1[index1]);//.second; 返回second的引用
            }
            int32_t index2 = my_hash2(key);
            if (bitset2[index2] && hashTable2[index2].first == key) {
                return std::get<1>(hashTable2[index2]);//.second;
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