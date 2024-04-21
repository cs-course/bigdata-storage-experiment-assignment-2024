#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <random>

class CuckooFilter {
private:
    static const int NUM_BUCKETS = 10; // 桶的数量
    static const int BUCKET_SIZE = 4; // 每个桶的大小
    std::vector<std::string> T1[NUM_BUCKETS]; // 第一个哈希表
    std::vector<std::string> T2[NUM_BUCKETS]; // 第二个哈希表
    std::hash<std::string> hashFunction1; // 哈希函数1
    std::hash<std::string> hashFunction2; // 哈希函数2
    std::mt19937 rng; // 用于生成随机数的随机数引擎
    std::uniform_int_distribution<int> distribution; // 均匀分布

    // 计算数据应该插入的两个桶的索引
    void getIndices(const std::string& item, int& index1, int& index2) {
        index1 = hashFunction1(item) % NUM_BUCKETS;
        index2 = (hashFunction2(item) % (NUM_BUCKETS - 1)) + 1;
    }

    // 随机选择一个哈希表进行替换
    int chooseTable() {
        return distribution(rng) % 2;
    }

    // 插入元素到指定的哈希表
    bool insertToTable(const std::string& item, std::vector<std::string>* table) {
    for (int i = 0; i < BUCKET_SIZE; ++i) {
        if (i >= table->size()) {
            table->push_back(item); // 如果向量大小不足，先添加足够的空字符串
        }
        if (table->at(i).empty()) {
            table->at(i) = item;
            return true;
        }
    }
    return false;
}


public:
    CuckooFilter() : rng(std::random_device()()), distribution(0, 1) {}

    // 插入数据
    bool insert(const std::string& item) {
        int index1, index2;
        getIndices(item, index1, index2);

        if (insertToTable(item, &T1[index1]))
            return true;
        if (insertToTable(item, &T2[index2]))
            return true;

        // 随机选择一个哈希表进行替换
        int tableIndex = chooseTable();
        std::vector<std::string>* table;
        if (tableIndex == 0)
            table = &T1[index1];
        else
            table = &T2[index2];

        // 随机选择一个桶进行替换
        int bucketIndex = distribution(rng) % BUCKET_SIZE;
        std::string temp = table->at(bucketIndex);
        table->at(bucketIndex) = item;

        // 尝试将替换出的元素插入另一个哈希表
        if (tableIndex == 0)
            return insertToTable(temp, &T2[index2]);
        else
            return insertToTable(temp, &T1[index1]);
    }

    // 检查数据是否存在
    bool contains(const std::string& item) {
        int index1, index2;
        getIndices(item, index1, index2);

        for (int i = 0; i < BUCKET_SIZE; ++i) {
            if (std::find(T1[index1].begin(), T1[index1].end(), item) != T1[index1].end() ||
                std::find(T2[index2].begin(), T2[index2].end(), item) != T2[index2].end()) {
                return true;
            }
        }
        return false;
    }

    // 返回哈希表 T1
    std::vector<std::string>* getT1() {
        return T1;
    }

    // 返回哈希表 T2
    std::vector<std::string>* getT2() {
        return T2;
    }
};


int main() {
    CuckooFilter filter;

    filter.insert("apple");
    filter.insert("banana");
    filter.insert("orange");

    std::cout << std::boolalpha;
    std::cout << "Contains apple? " << filter.contains("apple") << std::endl;
    std::cout << "Contains grape? " << filter.contains("grape") << std::endl;

    return 0;
}
