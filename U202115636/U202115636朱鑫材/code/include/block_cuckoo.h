#pragma once
#include "cuckoo.h"
#include <memory>
#include <vector>
#include <unordered_set>

extern uint32_t BLOCK_SIZE;             // 块的大小

struct Block {
    std::shared_ptr<std::vector<node> > nodes;

    Block() : nodes(std::make_shared<std::vector<node> >(BLOCK_SIZE)) {
        for (auto &node: *nodes) {
            node.is_empty = true;
            node.key = 0;
        }
    }
};

class BlockedCuckoo : public Cuckoo {
public:
    Block* blocks_{};
    Block* blocks_backup_{};
    std::unordered_set<uint32_t> keys_;
    uint32_t block_size_{};

    BlockedCuckoo(int capacity, const std::vector<HashType> &hash_types);
    ~BlockedCuckoo();
    void initBlocks() const;
    void backupBlocks() const;
    bool insert(uint32_t key) override;
    bool insert_without_search(uint32_t key) override;

    bool lookup(uint32_t key) override;

    void remove(uint32_t key) override;

    bool kick(const uint32_t &key, const uint32_t &loop) override;
    bool kick(const uint32_t &key, const uint32_t &loop, const uint32_t &prev_key) override;
    bool rehash() override;
};