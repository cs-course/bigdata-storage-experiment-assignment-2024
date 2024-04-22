#include <iostream>
#include <bitset>
#include <string>
#include <fstream>
#include <array>
#include <cmath>
using namespace std;

// 哈希函数结构体
struct Hash {
    Hash(size_t initial) : initialValue(initial) {}

    size_t operator()(const string& s) const {
        size_t hash = initialValue;
        for (char c : s) {
            hash = (hash * 131) + c;
        }
        return hash;
    }

    size_t initialValue;
};

// BloomFilter
template<size_t N>
class BloomFilter {
public:
    BloomFilter(size_t initial1, size_t initial2, size_t initial3, double indexCoefficient) 
        : hashes({Hash(initial1), Hash(initial2), Hash(initial3)}), indexCoefficient(indexCoefficient) {
        initializeIndexBits();
    }

    void add(const string& key1, const string& key2) {
        size_t index1 = hashes[0](key1) % N;
        size_t index2 = hashes[1](key1) % N;
        size_t index3 = hashes[2](key1) % N;

        size_t index4 = hashes[0](key2) % N;
        size_t index5 = hashes[1](key2) % N;
        size_t index6 = hashes[2](key2) % N;

        // 设置索引位为1
        indexBitset[index1] = true;
        indexBitset[index2] = true;
        indexBitset[index3] = true;
        indexBitset[index4] = true;
        indexBitset[index5] = true;
        indexBitset[index6] = true;

        // 根据索引系数设置属性位
        setAttributeBits(index1);
        setAttributeBits(index2);
        setAttributeBits(index3);
        setAttributeBits(index4);
        setAttributeBits(index5);
        setAttributeBits(index6);
    }

    bool contains(const string& key1, const string& key2) const {
        size_t index1 = hashes[0](key1) % N;
        size_t index2 = hashes[1](key1) % N;
        size_t index3 = hashes[2](key1) % N;

        size_t index4 = hashes[0](key2) % N;
        size_t index5 = hashes[1](key2) % N;
        size_t index6 = hashes[2](key2) % N;

        // 检查索引位是否都为1
        if (!indexBitset[index1] || !indexBitset[index2] || !indexBitset[index3] || 
            !indexBitset[index4] || !indexBitset[index5] || !indexBitset[index6]) {
            return false;
        }
        // 检查属性位是否都为1
        if (!attributeBitset[index1] || !attributeBitset[index2] || !attributeBitset[index3] ||
            !attributeBitset[index4] || !attributeBitset[index5] || !attributeBitset[index6]) {
            return false;
        }
        return true;
    }

private:
    array<Hash, 3> hashes; // 三个哈希函数
    bitset<N> indexBitset; // 索引位集合
    bitset<N> attributeBitset; // 属性位集合
    double indexCoefficient; // 索引系数

    // 根据索引系数设置属性位
    void setAttributeBits(size_t index) {
        size_t numAttributeBits = static_cast<size_t>(ceil(indexCoefficient * log(N)));
        for (size_t i = 0; i < numAttributeBits; ++i) {
            attributeBitset[(index + i) % N] = true;
        }
    }

    // 初始化索引位集合
    void initializeIndexBits() {
        indexBitset.reset();
    }
};

void testBloomFilter() {
    BloomFilter<75000> bf(123, 456, 789, 0.8); // 设置三个哈希函数的初始值，索引系数为0.8

    ifstream infile("Dataset.txt");
    string str1, str2;
    int count = 0, total = 0;

    // 从文件读取数据并进行测试
    while (infile >> str1 >> str2) {
        if (bf.contains(str1, str2)) {
            count++; // 误判
        }
        bf.add(str1, str2);
        total += 1;
    }

    cout << "Total items tested: " << total << endl;
    cout << "The number of false positives: " << count << endl;
    cout << "Error rate: " << (double)count / total << endl;

    infile.close();
}

int main() {
    testBloomFilter();
    return 0;
}
