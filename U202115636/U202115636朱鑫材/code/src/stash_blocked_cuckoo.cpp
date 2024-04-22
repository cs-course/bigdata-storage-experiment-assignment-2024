//
// Created by Pygone on 24-4-10.
//
#include "stash_blocked_cuckoo.h"
#include "data_generater.h"
#include "stash_cuckoo.h"
#include <cstdint>

extern uint32_t stash_size;

StashBlockedCuckoo::StashBlockedCuckoo(int capacity,
                                       const std::vector<HashType> &hash_types)
        : BlockedCuckoo(capacity-stash_size, hash_types) {
    stash_array_ = std::make_shared<std::unordered_set<uint32_t>>();
    stash_array_->reserve(stash_size);
}

bool StashBlockedCuckoo::insert(uint32_t key) {
    if (lookup(key)) {
        return true;
    }
    bool result = BlockedCuckoo::insert_without_search(key);
    if (!result && stash_array_->size() < stash_size) {
        stash_array_->insert(key);
        result = true;
    }
    return result;
}

void StashBlockedCuckoo::remove(uint32_t key) {
    if (BlockedCuckoo::lookup(key)) {
        BlockedCuckoo::remove(key);
        return;
    }
    auto iter = stash_array_->find(key);
    if (iter != stash_array_->end()) {
        stash_array_->erase(iter);
    }
}

bool StashBlockedCuckoo::lookup(uint32_t key) {
    return BlockedCuckoo::lookup(key) || stash_array_->count(key);
}

bool StashBlockedCuckoo::rehash() {
    auto old_blocks = blocks_;
    auto old_size = size_;
    auto old_stash_array = stash_array_;
    blocks_ = new Block[block_size_];
    stash_array_ = std::make_shared<std::unordered_set<uint32_t>>();
    stash_array_->reserve(stash_size);
    for (auto &hash: hashs_) {
        hash->set_seed(DataGenerater::generate());
    }
    size_ = 0;
    bool success = true;
    for (int i = 0; i < block_size_; i++) {
        auto &block = old_blocks[i];
        for (int j = 0; j < BLOCK_SIZE; j++) {
            auto &node = block.nodes->at(j);
            if (!node.is_empty) {
                if (!insert_without_search(node.key)) {
                    success = false;
                    break;
                }
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
        auto to_delete = blocks_;
        blocks_ = old_blocks;
        size_ = old_size;
        stash_array_ = old_stash_array;
        delete[] to_delete;
    } else {
        delete[] old_blocks;
    }
    return success;
}

bool StashBlockedCuckoo::insert_without_search(uint32_t key) {
    bool result = BlockedCuckoo::insert_without_search(key);
    if (!result && stash_array_->size() < stash_size) {
        stash_array_->insert(key);
        result = true;
    }
    return result;
}
