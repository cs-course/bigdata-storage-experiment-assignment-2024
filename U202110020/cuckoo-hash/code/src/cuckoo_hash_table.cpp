#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <random>
#include <utility>
#include <vector>

#include "spdlog/sinks/stdout_color_sinks.h"

template <typename K, typename V>
class CuckooHashTable {
    static const int default_table_size = 100;
    static const int default_max_retry = 100;
    static const int default_entry_cnt = 5;

   private:
    std::shared_ptr<spdlog::logger> logger =
        spdlog::stderr_color_mt("cuckoo_hash_table");
    std::vector<std::pair<K, V>> table1;
    std::vector<std::pair<K, V>> table2;
    std::vector<bool> null_bits1;
    std::vector<bool> null_bits2;
    size_t table_size;
    uint32_t max_retry;
    uint32_t entry_cnt;

   public:
    CuckooHashTable() {
        table_size = default_table_size;
        max_retry = default_max_retry;
        table1.resize(default_table_size);
        table2.resize(default_table_size);
        null_bits1.resize(default_table_size, true);
        null_bits2.resize(default_table_size, true);
    }
    CuckooHashTable(size_t size_ = default_table_size,
                    uint32_t max_retry_ = default_max_retry) {
        table_size = size_;
        max_retry = max_retry_;
        table1.resize(size_);
        table2.resize(size_);
        null_bits1.resize(size_, true);
        null_bits2.resize(size_, true);
    }

    V find(const K& key) {
        V val;
        if (lookup(key, val)) {
            return val;
        } else {
            // throw std::out_of_range("Key not found");
            return V{};
        }
    }

    bool insert(const K& key_, const V& val_) {
        size_t hash1 = get_first_hash(key_);
        size_t hash2 = get_second_hash(key_);
        K key = key_;
        V val = val_;
        for (int i = 0; i < max_retry; ++i) {
            if (null_bits1[hash1]) {
                table1[hash1] = make_pair(key, val);
                null_bits1[hash1] = false;
                // std::cout << "inserted into table1: hash: " << hash1
                //           << ", key: " << key << ", val: " << val << std::endl;
                // return true;
            } else if (null_bits2[hash2]) {
                table2[hash2] = make_pair(key, val);
                null_bits2[hash2] = false;
                // std::cout << "inserted into table2: hash: " << hash2
                //           << ", key: " << key << ", val: " << val << std::endl;

                return true;
            } else {
                std::uniform_int_distribution<int> dist(1, 2);
                int table_index = dist(generator);
                if (table_index == 1) {
                    K evict_key = table1[hash1].first;
                    V evict_val = table1[hash1].second;
                    table1[hash1] = make_pair(key, val);
                    key = evict_key;
                    val = evict_val;
                    hash1 = get_first_hash(key);
                    std::cout << "evicted from table1: hash" << hash1
                              << ", key: " << key << ", val: " << val
                              << std::endl;
                } else {
                    K evict_key = table2[hash2].first;
                    V evict_val = table2[hash2].second;
                    table2[hash2] = make_pair(key, val);
                    key = evict_key;
                    val = evict_val;
                    hash2 = get_second_hash(key);
                    std::cout << "evicted from table2: hash" << hash2
                              << ", key: " << key << ", val: " << val
                              << std::endl;
                }
            }
        }

        // If we reach here, it means we have tried max_retry times and failed
        // to insert, so we need to resize the hash table
        resize_hash_table();
        return insert(key, val);
    }

    bool remove(const K& key) {
        size_t hash1 = get_first_hash(key);
        size_t hash2 = get_second_hash(key);
        if (table1[hash1].first == key) {
            null_bits1[hash1] = true;
            return true;
        } else if (table2[hash2].first == key) {
            null_bits2[hash2] = true;
            return true;
        }
        return false;
    }

    void resize_hash_table() {
        table_size *= 2;
        logger->info("Resizing hash table to {}", table_size);
        std::vector<std::pair<K, V>> new_table1(table_size);
        std::vector<std::pair<K, V>> new_table2(table_size);
        std::vector<bool> new_null_bits1(table_size, true);
        std::vector<bool> new_null_bits2(table_size, true);
        for (size_t i = 0; i < table1.size(); ++i) {
            if (!null_bits1[i]) {
                K key = table1[i].first;
                V val = table1[i].second;
                size_t hash1 = get_first_hash(key);
                if (new_null_bits1[hash1]) {
                    new_table1[hash1] = make_pair(key, val);
                    new_null_bits1[hash1] = false;
                } else {
                    logger->error("Failed to resize hash table");
                    throw std::runtime_error("Failed to resize hash table");
                }
            }
            if (!null_bits2[i]) {
                K key = table2[i].first;
                V val = table2[i].second;
                size_t hash2 = get_second_hash(key);
                if (new_null_bits2[hash2]) {
                    new_table2[hash2] = make_pair(key, val);
                    new_null_bits2[hash2] = false;
                } else {
                    logger->error("Failed to resize hash table");
                    throw std::runtime_error("Failed to resize hash table");
                }
            }
        }
        table1.clear();
        table2.clear();
        null_bits1.clear();
        null_bits2.clear();

        table1 = new_table1;
        table2 = new_table2;
        null_bits1 = new_null_bits1;
        null_bits2 = new_null_bits2;
    }

    int get_table_size() const { return 2 * table_size; }

   private:
    std::default_random_engine generator;

    uint32_t get_first_hash(const K& key) {
        return std::hash<K>{}(key) % table_size;
    }

    uint32_t get_second_hash(const K& key) {
        return (std::hash<K>{}(key) * 2654435761) % table_size;
    }

    bool lookup(const K& key, V& val) {
        size_t hash1 = get_first_hash(key);
        size_t hash2 = get_second_hash(key);
        if (!null_bits1[hash1] && table1[hash1].first == key) {
            val = table1[hash1].second;
            return true;
        } else if (!null_bits2[hash2] && table2[hash2].first == key) {
            val = table2[hash2].second;
            return true;
        }
        return false;
    }
};