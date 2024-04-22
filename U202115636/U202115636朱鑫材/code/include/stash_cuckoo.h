#include "cuckoo.h"
#include <cstdint>
#include <unordered_set>
#include <vector>

class StashCuckoo : public Cuckoo {
    std::shared_ptr<std::unordered_set<uint32_t>> stash_array_;

public:
    StashCuckoo(int capacity, const std::vector<HashType> &hash_types);

    bool insert(uint32_t key) override;

    bool insert_without_search(uint32_t key) override;

    void remove(uint32_t key) override;

    bool lookup(uint32_t key) override;

    bool rehash() override;
};