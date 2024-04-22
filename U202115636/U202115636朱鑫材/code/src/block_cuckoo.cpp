//
// Created by Pygon on 24-4-10.
//
#include "block_cuckoo.h"
#include "data_generater.h"
#include <algorithm>
#include <stdexcept>

BlockedCuckoo::BlockedCuckoo(int capacity,
                             const std::vector<HashType> &hash_types)
        : Cuckoo(hash_types) {
    block_size_ = capacity / BLOCK_SIZE;
    blocks_ = new Block[block_size_];
    blocks_backup_ = new Block[block_size_];
    capacity_ = capacity;
    size_ = 0;
    max_loop_ = 20;
}

BlockedCuckoo::~BlockedCuckoo() {
    delete[] blocks_;
    delete[] blocks_backup_;
}

bool BlockedCuckoo::insert(uint32_t key) {
    if (lookup(key)) {
        return true;
    }
    if (size_ == capacity_) {
        throw std::runtime_error("Table is full");
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.is_empty) {
                node.key = key;
                node.is_empty = false;
                size_++;
                return true;
            }
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (kick(node.key, 0)) {
                node.key = key;
                return true;
            }
        }
        return false;
    });
}

bool BlockedCuckoo::kick(const uint32_t &key, const uint32_t &loop) {
    if (loop >= max_loop_) {
        return false;
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.is_empty) {
                node.key = key;
                node.is_empty = false;
                size_++;
                return true;
            }
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.key == key) {
                continue;
            }
            if (kick(node.key, loop + 1, key)) {
                node.key = key;
                return true;
            }
        }
        return false;
    });
}

bool BlockedCuckoo::kick(const uint32_t &key, const uint32_t &loop, const uint32_t &prev_key) {
    if (loop >= max_loop_) {
        return false;
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.is_empty) {
                node.key = key;
                node.is_empty = false;
                size_++;
                return true;
            }
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.key == key || node.key == prev_key) {
                continue;
            }
            if (kick(node.key, loop + 1, key)) {
                node.key = key;
                return true;
            }
        }
        return false;
    });
}

bool BlockedCuckoo::rehash() {
    auto old_size = size_;
    initBlocks();
    for (auto &hash: hashs_) {
        hash->set_seed(DataGenerater::generate());
    }
    size_ = 0;
    bool success = true;
    for (int i = 0; i < block_size_; i++) {
        auto &block = blocks_backup_[i];
        for (auto &node: *block.nodes) {
            if (!node.is_empty) {
                if (!insert_without_search(node.key)) {
                    success = false;
                    break;
                }
            }
        }
        if (!success) {
            break;
        }
    }
    if (!success) {
        backupBlocks();
        size_ = old_size;
    }
    return success;
}

bool BlockedCuckoo::insert_without_search(uint32_t key) {
    if (size_ == capacity_) {
        throw std::runtime_error("Table is full");
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (node.is_empty) {
                node.key = key;
                node.is_empty = false;
                size_++;
                return true;
            }
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            auto &node = block.nodes->at(i);
            if (kick(node.key, 0)) {
                node.key = key;
                return true;
            }
        }
        return false;
    });

}


void BlockedCuckoo::initBlocks() const {
    // copy blocks to blocks_backup_
    for (int i = 0; i < block_size_; i++) {
        auto &block = blocks_[i];
        for (int j = 0; j < BLOCK_SIZE; j++) {
            auto &node = block.nodes->at(j);
            auto &node_backup = blocks_backup_[i].nodes->at(j);
            node_backup.is_empty = node.is_empty;
            node_backup.key = node.key;
        }
    }
    // clear blocks
    for (int i = 0; i < block_size_; i++) {
        auto &block = blocks_[i];
        for (auto &node: *block.nodes) {
            node.is_empty = true;
            node.key = 0;
        }
    }
}

void BlockedCuckoo::backupBlocks() const {
    for (int i = 0; i < block_size_; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            blocks_[i].nodes->at(j) = blocks_backup_[i].nodes->at(j);
        }
    }
}

bool BlockedCuckoo::lookup(uint32_t key) {
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        return std::any_of(
                (*block.nodes).begin(), (*block.nodes).end(),
                [&](auto &node) { return !node.is_empty && node.key == key; });
    });
}

void BlockedCuckoo::remove(uint32_t key) {
    if (!lookup(key)) {
        return;
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % block_size_;
        Block &block = blocks_[index];
        for (auto &node: *block.nodes) {
            if (!node.is_empty && node.key == key) {
                node.is_empty = true;
                size_--;
                return;
            }
        }
    }
}






