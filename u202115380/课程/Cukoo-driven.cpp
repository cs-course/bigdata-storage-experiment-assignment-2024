#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cstring> // for memcpy
using namespace std;

// MurmurHash3

void MurmurHash3_x86_32(const void* key, int len, uint32_t seed, void* out) {
    const uint8_t* data = (const uint8_t*)key;
    const int nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Body
    const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1,15);
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);  // ROTL32(h1,13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
    uint32_t k1 = 0;
    switch (len & 3) {
    case 3:
        k1 ^= tail[2] << 16;
        [[fallthrough]];
    case 2:
        k1 ^= tail[1] << 8;
        [[fallthrough]];
    case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1,15);
        k1 *= c2;
        h1 ^= k1;
    };

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    *(uint32_t*)out = h1;
}

// FNV-1a
size_t fnv1a(const void* key, size_t len) {
    const unsigned char* data = (const unsigned char*)key;
    const size_t prime = 0x100000001B3ULL;
    size_t hash = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

#define TABLE_SIZE 1000
#define MAX_KICKS 1000
#define LOAD_FACTOR_THRESHOLD 0.75 // 负载因子阈值
#define TABLE_RESIZE_FACTOR 2 // 哈希表调整因子

class CuckooHashTable {
private:
    vector<vector<int>> tables; // 使用向量的向量来表示多个哈希表
    int size; // 哈希表的大小
    int numKeys; // 哈希表中键的数量

    // 使用 MurmurHash3
    size_t hash(int k, int tableIdx) {
        size_t seed = tableIdx * 0x01000193; // 此处可以调整种子以改变哈希函数的行为
        size_t hashValue;
        MurmurHash3_x86_32(&k, sizeof(k), seed, &hashValue);
        return hashValue % size;
    }

    // 动态调整哈希表大小
    void resize() {
        int newSize = TABLE_RESIZE_FACTOR * size;
        vector<vector<int>> newTables(2, vector<int>(newSize, -1));

        // 重新哈希并插入所有键
        for (auto& table : tables) {
            for (int key : table) {
                int tableIdx = (&table - &tables[0]);
                size_t hashValue = hash(key, tableIdx);
                newTables[tableIdx][hashValue] = key;
            }
        }

        size = newSize;
        tables = std::move(newTables);
    }

public:
    CuckooHashTable() {
        size = TABLE_SIZE;
        numKeys = 0;
        tables.resize(2, vector<int>(size, -1));
    }

    bool insert(int key) {
        if ((double)numKeys / size >= LOAD_FACTOR_THRESHOLD)
            resize(); // 如果负载因子超过阈值，调整哈希表大小

        return insertHelper(key, 0);
    }

    bool insertHelper(int key, int tableIdx, int cnt = 0) {
        if (cnt > MAX_KICKS) return false;

        size_t hashValue = hash(key, tableIdx);

        if (tables[tableIdx][hashValue] == -1) {
            tables[tableIdx][hashValue] = key;
            numKeys++;
            return true;
        } else {
            int displacedKey = tables[tableIdx][hashValue];
            tables[tableIdx][hashValue] = key;
            return insertHelper(displacedKey, (tableIdx + 1) % 2, cnt + 1);
        }
    }

    bool search(int key) {
        size_t hashValue1 = hash(key, 0);
        size_t hashValue2 = hash(key, 1);

        return (tables[0][hashValue1] == key || tables[1][hashValue2] == key);
    }
};

vector<int> random_creator() {
    random_device rd;
    mt19937 gen(rd());

    vector<int> sequence(10000);
    iota(sequence.begin(), sequence.end(), 1);

    shuffle(sequence.begin(), sequence.end(), gen);

    vector<int> randoms(sequence.begin(), sequence.begin() + 1000);
    return randoms;
}

int main() {
    double average_time = 0;
    double average_false = 0;

    for (int j = 0; j < 1000; j++) {
        CuckooHashTable hashTable;
        vector<int> randoms = random_creator();

        auto start = chrono::high_resolution_clock::now();
        for (int i : randoms) {
            hashTable.insert(i);
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);

        int false_cnt = 0;
        for (int i : randoms) {
            if (!hashTable.search(i))
                false_cnt++;
        }

        average_time += duration.count();
        average_false += false_cnt;
    }

    average_time /= 1000;
    average_false /= 1000;

    cout << "插入完成时间为：" << average_time << " nanoseconds" << endl;
    cout << "平均无法插入率为：" << average_false;

    return 0;
}
