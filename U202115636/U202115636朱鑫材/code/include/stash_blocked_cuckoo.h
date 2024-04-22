//
// Created by Pygon on 24-4-10.
//
#include "block_cuckoo.h"
#include <unordered_set>

class StashBlockedCuckoo : public BlockedCuckoo {

public:
  std::shared_ptr<std::unordered_set<uint32_t>> stash_array_;

  StashBlockedCuckoo(int capacity, const std::vector<HashType> &hash_types);

  bool insert(uint32_t key) override;

  bool insert_without_search(uint32_t key) override;

  void remove(uint32_t key) override;

  bool lookup(uint32_t key) override;

  bool rehash() override;
};
