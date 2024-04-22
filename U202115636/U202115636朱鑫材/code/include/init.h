//
// Created by Pygon on 24-4-13.
//
#include "data_generater.h"
#include "stash_blocked_cuckoo.h"
#include "stash_cuckoo.h"
#include <csignal>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <unordered_set>
#include <iomanip>

enum class CuckooType {
    Cuckoo = 0,
    StashCuckoo = 1,
    BlockedCuckoo = 2,
    StashBlockedCuckoo = 3
};
std::unordered_map<CuckooType, std::string> cuckoo_type_map = {
        {CuckooType::Cuckoo,             "Cuckoo"},
        {CuckooType::StashCuckoo,        "StashCuckoo"},
        {CuckooType::BlockedCuckoo,      "BlockedCuckoo"},
        {CuckooType::StashBlockedCuckoo, "StashBlockedCuckoo"}
};

std::vector<HashType> generate_hash_types(int num) {
    std::vector<HashType> hash_types;
    for (int i = 0; i < num; i++) {
        auto key = DataGenerater::generate() % 2;
        if (key == 0) {
            hash_types.push_back(HashType::Jenkins);
        } else {
            hash_types.push_back(HashType::Murmur);
        }
    }
    return hash_types;
}


std::shared_ptr<Cuckoo> build_cuckoo(CuckooType type, int capacity, int hash_types) {
    switch (type) {
        case CuckooType::Cuckoo:
            return std::make_shared<Cuckoo>(capacity, generate_hash_types(hash_types)
            );
        case CuckooType::StashCuckoo:
            return std::make_shared<StashCuckoo>(capacity, generate_hash_types(hash_types)
            );
        case CuckooType::BlockedCuckoo:
            return std::make_shared<BlockedCuckoo>(capacity, generate_hash_types(hash_types)
            );
        case CuckooType::StashBlockedCuckoo:
            return std::make_shared<StashBlockedCuckoo>(capacity, generate_hash_types(hash_types)
            );
        default:
            return nullptr;
    }
}

