//
// Created by Pygon on 24-4-10.
//
#include "stash_cuckoo.h"
#include "cuckoo.h"
#include "data_generater.h"
#include <cstdint>

extern uint32_t stash_size;

StashCuckoo::StashCuckoo(int capacity, const std::vector<HashType> &hash_types)
        : Cuckoo(capacity-stash_size, hash_types) {
    stash_array_ = std::make_shared<std::unordered_set<uint32_t>>();
    stash_array_->reserve(stash_size);
}

bool StashCuckoo::insert(uint32_t key) {
    if (lookup(key)) {
        return true;
    }
    bool result = Cuckoo::insert_without_search(key);
    if (!result && stash_array_->size() < stash_size) {
        stash_array_->insert(key);
        result = true;
    }
    return result;
}

void StashCuckoo::remove(uint32_t key) {
    if (Cuckoo::lookup(key)) {
        Cuckoo::remove(key);
        return;
    }
    auto iter = stash_array_->find(key);
    if (iter != stash_array_->end()) {
        stash_array_->erase(iter);
    }
}

bool StashCuckoo::lookup(uint32_t key) {
    return Cuckoo::lookup(key) || stash_array_->count(key);
}

bool StashCuckoo::rehash() {
    auto old_table = table_;
    auto old_size = size_;
    auto old_stash_array = stash_array_;
    table_ = new node[capacity_];
    stash_array_ = std::make_shared<std::unordered_set<uint32_t>>(stash_size);
    for (auto &hash: hashs_) {
        hash->set_seed(DataGenerater::generate());
    }
    size_ = 0;
    bool success = true;
    for (int i = 0; i < capacity_; i++) {
        auto &node = old_table[i];
        if (!node.is_empty) {
            if (!insert_without_search(node.key)) {
                success = false;
                break;
            }
        }
    }
    if (success) {
        for (auto &key: *old_stash_array) {
            if (!insert_without_search(key)) {
                success = false;
                break;
            }
        }
    }
    if (!success) {
        table_ = old_table;
        size_ = old_size;
        stash_array_ = old_stash_array;
    }
    return success;
}

bool StashCuckoo::insert_without_search(uint32_t key) {
    bool result = Cuckoo::insert_without_search(key);
    if (!result && stash_array_->size() < stash_size) {
        stash_array_->insert(key);
        result = true;
    }
    return result;
}
