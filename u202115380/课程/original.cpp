#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <algorithm>
#include<numeric>
#include <chrono>
using namespace std;

#define TABLE_SIZE 1000
#define MAX_KICKS 10

int hash1(int k) {
    return k /10;
}

int hash2(int k) {
    return k + 1;
}

class CuckooHashTable {
private:
    vector<int> table1;
    vector<int> table2;
public:
    CuckooHashTable() {//提供初始化函数
        table1.resize(TABLE_SIZE, -1);
        table2.resize(TABLE_SIZE, -1);
    }

    bool insert(int key) {
        return insertHelper(key, 0);
    }

    bool insertHelper(int key, int tableIdx, int cnt = 0) {//key为放入的数据，tableIdx为当前处理的hash表，cnt为当前踢了第几次
        if (cnt > MAX_KICKS) return false;
        if (tableIdx == 0) {//对hash表1进行判断
            size_t hashValue = hash1(key) % TABLE_SIZE;
            if (table1[hashValue] == -1) {//hash表1并未发生冲突，放入数据
                table1[hashValue] = key;
                return true;
            }
            else {//hash表1发生冲突，将1的数据踢出
                int displacedKey = table1[hashValue];
                table1[hashValue] = key;
                return insertHelper(displacedKey, 1, cnt + 1);
            }
        }
        else {
            size_t hashValue = hash2(key) % TABLE_SIZE;
            if (table2[hashValue] == -1) {
                table2[hashValue] = key;
                return true;
            }
            else {
                int displacedKey = table2[hashValue];
                table2[hashValue] = key;
            }
        }
    }

    bool search(int key) {
        size_t hashValue1 = hash1(key) % TABLE_SIZE;
        size_t hashValue2 = hash2(key) % TABLE_SIZE;
        if (table1[hashValue1] == key || table2[hashValue2] == key)//检查hash表1和2，是否存储该数据
            return true;
        else
            return false;
    }
};

vector<int> random_creator() {
    random_device rd;
    mt19937 gen(rd());

    // 生成从1到10000的序列
    vector<int> sequence(10000);
    vector<int> randoms(0);
    iota(sequence.begin(), sequence.end(), 1);
    // 打乱序列
    shuffle(sequence.begin(), sequence.end(), gen);
    copy(sequence.begin(), sequence.begin() + 1000, back_inserter(randoms));
    return randoms;
}


int main() {
    
    
    double average_time = 0;//平均值
    double average_false = 0;//平均错误个数
    double totaltime = 0;
    double sum_false = 0;
    
    int j;
    for (j = 0; j < 10000; j++) {
        CuckooHashTable hashTable;
        //创造随机数
        vector<int> randoms;
        randoms = random_creator();
        // 插入随机数
        auto start = chrono::high_resolution_clock::now();
        for (int i : randoms) {
            hashTable.insert(i);
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
        totaltime += duration.count();
        int false_cnt = 0;
        // 查询一些元素,统
        for (int i : randoms) {
            if (hashTable.search(i) == false) false_cnt++;
        }
        cout << false_cnt << endl;
        sum_false += false_cnt;
    }
    average_false = sum_false / 10000;
    average_time = totaltime / 10000;
    cout << "插入完成时间为：" << average_time << " nanoseconds" << endl;
    cout << "平均无法插入率为：" << average_false;
    return 0;
}
