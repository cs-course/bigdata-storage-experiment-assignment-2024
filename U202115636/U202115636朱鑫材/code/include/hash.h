#pragma once
#include <cstdint>

enum class HashType {
    Jenkins, Murmur
};

class Hash {

public:
    uint32_t seed_{};

    virtual uint32_t operator()(uint32_t key) const = 0;

    virtual void set_seed(uint32_t seed) = 0;

    virtual ~Hash() = default;

    virtual uint32_t get_seed() = 0;
};

class JenkinsHash : public Hash {

public:
    JenkinsHash() = default;

    uint32_t get_seed() override { return seed_; }

    uint32_t operator()(uint32_t key) const override {
        key += seed_;
        key += (key << 12);
        key ^= (key >> 22);
        key += (key << 4);
        key ^= (key >> 9);
        key += (key << 10);
        key ^= (key >> 2);
        key += (key << 7);
        key ^= (key >> 12);
        return key;
    }

    void set_seed(uint32_t seed) override { seed_ = seed; }
};

class MurmurHash3 : public Hash {

public:
    MurmurHash3() = default;
    uint32_t get_seed() override { return seed_; }

    uint32_t operator()(uint32_t key) const override {
        key ^= seed_;
        key ^= key >> 16;
        key *= 0x85ebca6b;
        key ^= key >> 13;
        key *= 0xc2b2ae35;
        key ^= key >> 16;
        return key;
    }

    void set_seed(uint32_t seed) override { seed_ = seed; }
};