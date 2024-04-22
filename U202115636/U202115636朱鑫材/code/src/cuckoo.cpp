
#include <algorithm>
#include "cuckoo.h"
#include "data_generater.h"

extern int count;
extern int count2;

Cuckoo::Cuckoo(int capacity, const std::vector<HashType> &hash_types) {
    capacity_ = capacity;
    table_ = new node[capacity_];
    size_ = 0;
    max_loop_ = 20;
    for (auto hash_type: hash_types) {
        if (hash_type == HashType::Jenkins) {
            hashs_.emplace_back(std::make_shared<JenkinsHash>());
        } else {
            hashs_.emplace_back(std::make_shared<MurmurHash3>());
        }
    }
    for (auto &hash: hashs_) {
        hash->set_seed(DataGenerater::generate());
    }
}

Cuckoo::Cuckoo(const std::vector<HashType> &hash_types) {
    capacity_ = 0;
    size_ = 0;
    max_loop_ = 20;
    hashs_.reserve(hash_types.size());
    for (auto hash_type: hash_types) {
        if (hash_type == HashType::Jenkins) {
            hashs_.emplace_back(std::make_shared<JenkinsHash>());
        } else {
            hashs_.emplace_back(std::make_shared<MurmurHash3>());
        }
    }
    for (auto &hash: hashs_) {
        hash->set_seed(DataGenerater::generate());
    }
}

Cuckoo::~Cuckoo() {
    delete[] table_;
}

bool Cuckoo::insert(uint32_t key) {
    if (lookup(key)) {
        return true;
    }
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % capacity_;
        if (table_[index].is_empty) {
            table_[index].key = key;
            table_[index].is_empty = false;
            size_++;
            return true;
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % capacity_;
        auto origin_key = table_[index].key;
        if (kick(origin_key, 0)) {
            table_[index].key = key;
            return true;
        }
        return false;
    });
}


bool Cuckoo::kick(const uint32_t &key, const uint32_t &loop) {
    count2++;
    if (loop >= max_loop_) {
        return false;
    }
    for (auto &hash: hashs_) {
        uint32_t i = (*hash)(key) % capacity_;
        if (table_[i].is_empty) {
            table_[i].key = key;
            table_[i].is_empty = false;
            size_++;
            return true;
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t i = (*hash)(key) % capacity_;
        auto origin_key = table_[i].key;
        if (origin_key == key) {
            count++;
            return false;
        }
        if (kick(origin_key, loop + 1, key)) {
            table_[i].key = key;
            return true;
        }
        return false;
    });
}

bool Cuckoo::kick(const uint32_t &key, const uint32_t &loop, const uint32_t &prev_key) {
    count2++;
    if (loop >= max_loop_) {
        return false;
    }
    for (auto &hash: hashs_) {
        uint32_t i = (*hash)(key) % capacity_;
        if (table_[i].is_empty) {
            table_[i].key = key;
            table_[i].is_empty = false;
            size_++;
            return true;
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t i = (*hash)(key) % capacity_;
        auto origin_key = table_[i].key;
        if (origin_key == key || origin_key == prev_key) { // 自循环 和 之前的key
            count++;
            return false;
        }
        if (kick(origin_key, loop + 1, key)) {
            table_[i].key = key;
            return true;
        }
        return false;
    });
}

bool Cuckoo::lookup(uint32_t key) {
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % capacity_;
        return !table_[index].is_empty && table_[index].key == key;
    });
}

void Cuckoo::remove(uint32_t key) {
    if (!lookup(key)) {
        return;
    }
    size_--;
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % capacity_;
        if (!table_[index].is_empty && table_[index].key == key) {
            table_[index].is_empty = true;
            return;
        }
    }

}

void Cuckoo::set_max_loop(uint32_t loop) {
    max_loop_ = loop;
}

bool Cuckoo::rehash() {
    auto old_table = table_;
    auto old_size = size_;
    table_ = new node[capacity_];
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
    if (!success) {
        auto to_delete = table_;
        table_ = old_table;
        size_ = old_size;
        delete[] to_delete;
    } else {
        delete[] old_table;
    }
    return success;
}

bool Cuckoo::insert_without_search(uint32_t key) {
    for (auto &hash: hashs_) {
        uint32_t index = (*hash)(key) % capacity_;
        if (table_[index].is_empty) {
            table_[index].key = key;
            table_[index].is_empty = false;
            size_++;
            return true;
        }
    }
    return std::any_of(hashs_.begin(), hashs_.end(), [&](auto &hash) {
        uint32_t index = (*hash)(key) % capacity_;
        auto origin_key = table_[index].key;
        if (kick(origin_key, 0)) {
            table_[index].key = key;
            return true;
        }
        return false;
    });
}
