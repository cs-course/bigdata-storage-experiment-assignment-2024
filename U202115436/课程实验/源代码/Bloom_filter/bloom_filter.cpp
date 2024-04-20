#include <vector>
#include <array>
#include <bitset>
#include <functional>
#include <iostream>


class BloomFilter {
private:
    std::bitset<1024> bits; // 选择适当大小的 bitset
    std::hash<std::string> hash_fn1;
    std::hash<std::string> hash_fn2;

public:
    void add(const std::string& item) {
        auto hash1 = hash_fn1(item) % bits.size();
        auto hash2 = hash_fn2(item) % bits.size();
        bits.set(hash1);
        bits.set(hash2);
    }

    bool possiblyContains(const std::string& item) const {
        auto hash1 = hash_fn1(item) % bits.size();
        auto hash2 = hash_fn2(item) % bits.size();
        return bits.test(hash1) && bits.test(hash2);
    }
};


class UnionMultiDimensionalBloomFilter {
private:
    std::array<BloomFilter, 3> filters; // 假设有3个维度
    BloomFilter unionFilter; // 用于联合属性

public:
    void add(const std::array<std::string, 3>& items) {
        std::string combined;
        for (size_t i = 0; i < items.size(); ++i) {
            filters[i].add(items[i]);
            combined += items[i]; // 创建联合字符串
        }
        unionFilter.add(combined); // 添加联合字符串到联合 Bloom Filter
    }

    bool possiblyContains(const std::array<std::string, 3>& items) {
        std::string combined;
        for (size_t i = 0; i < items.size(); ++i) {
            if (!filters[i].possiblyContains(items[i])) {
                return false;
            }
            combined += items[i];
        }
        return unionFilter.possiblyContains(combined); // 检查联合 Bloom Filter
    }
};


int main() {
    UnionMultiDimensionalBloomFilter umdbf;

    // 添加一些数据
    umdbf.add({ "apple", "banana", "cherry" });
    umdbf.add({ "dog", "elephant", "frog" });

    // 测试查询
    std::cout << "Testing 'apple', 'banana', 'cherry': "
        << (umdbf.possiblyContains({ "apple", "banana", "cherry" }) ? "Found" : "Not Found") << std::endl;
    std::cout << "Testing 'apple', 'banana', 'grape': "
        << (umdbf.possiblyContains({ "apple", "banana", "grape" }) ? "Found" : "Not Found") << std::endl;
    std::cout << "Testing 'apple', 'elephant', 'cherry': "
        << (umdbf.possiblyContains({ "apple", "elephant", "cherry" }) ? "Found" : "Not Found") << std::endl;
    return 0;
}

