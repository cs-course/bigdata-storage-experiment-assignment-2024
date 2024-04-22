#pragma once

#include "hash.h"
#include <cstdint>
#include <memory>
#include <vector>

struct node {
    uint32_t key{};
    bool is_empty = true;

    // get hash value of key
    bool operator==(const node &node) const { return key == node.key; }
};

struct nodeHash {
    uint64_t operator()(const node &node) const {
        return std::hash<uint32_t>{}(node.key);
    }
};

struct hashTable {
    std::vector<node> table;
    uint32_t size;
    uint32_t capacity;
};

class Cuckoo {
public:
    node *table_{}; // hash table
    std::vector<std::shared_ptr<Hash>> hashs_; // hash functions
    uint32_t capacity_{}; // capacity of hash table
    uint32_t size_{}; // size of hash table
    uint32_t max_loop_{}; // max loop

    Cuckoo(int capacity, const std::vector<HashType> &hash_types);

    ~Cuckoo();

    explicit Cuckoo(const std::vector<HashType> &hash_types);

    void set_max_loop(uint32_t loop);
    /**
     * insert key into cuckoo hash table
     * @param key
     * @return
     */
    virtual bool insert(uint32_t key);
    /**
     * insert key into cuckoo hash table without lookup
     * used in rehash
     * @param key
     * @return
     */
    virtual bool insert_without_search(uint32_t key);
    /**
     * remove key from cuckoo hash table
     * @param key
     */
    virtual void remove(uint32_t key);

    /**
     * lookup key in cuckoo hash table
     * @param key
     * @return
     */
    virtual bool lookup(uint32_t key);

    /**
     * rehash cuckoo hash table
     * @return
     */
    virtual bool rehash();

    /**
     * kick out key from cuckoo hash table
     * @param key
     * @param loop
     * @return
     */
    virtual bool kick(const uint32_t &key, const uint32_t &loop);
    virtual bool kick(const uint32_t &key, const uint32_t &loop,const uint32_t &prev_key);


};