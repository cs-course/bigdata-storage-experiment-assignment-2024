//
//  main.cpp
//  CoKoHash
//
//  Created by 王彬 on 2024/4/22.
//

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <cstring>
#include <functional>
#include <random>

using namespace std::literals::chrono_literals;

class CuckooMap {
private:
    const unsigned int ITER_ROUNDS_MAX = 15;    // Cuckoo Insert 循环次数最大值
    const unsigned int HASH_MAXN = 10;
    const unsigned int P = 1e9+7;
    int hash_key_primes[10] = {2,3,5,7,11,13,17,19,23,29}; // 质数，哈希函数使用
    const double conflict_rate = 0.5; // 扩容阈值比例
    int NUM_SLOTS = 4; // 槽位个数
    int NUM_BUCKETS = 2; // 桶的数量，即哈希函数的个数
    int BUCKET_SIZE = 1000; // 每个桶的大小
    
    // 结果输出
    int insert_failure = 0;
    int insert_total = 0;
    float time_consumed_for_reconstruction = 0.0;
    
    
    unsigned int *T; // 哈希表
    unsigned int *PT; // 每个桶内有多少元素已经存放
    std::vector<std::string> ValueTable;    // 值表，0作空
    std::hash<std::string> szHash;  // hash指纹函数

    std::mt19937 rng; // 用于生成随机数的随机数引擎
    std::uniform_int_distribution<int> distribution_alloc; // 均匀分布
    
    /*
     @func:HashF
     @params: 待哈希对象str，哈希选项num
     @Hash函数，满足0<=num<10，使用质数进行字符串哈希
     */
    unsigned int HashF(const std::string& str, int num){
        if (num < 0 || num >= 10){
            throw "Number of Hash Map cannot satisfy needs.";
        }
        unsigned int ret = 0;
        for (auto i:str){
            ret ^= i;
            ret <<= hash_key_primes[num];
        }
        return ret % BUCKET_SIZE;
    }
    
    /*
     @func:fingerPrint
     @params: 待哈希对象str
     @输出元素的哈希指纹
     */
    unsigned int fingerPrint(const std::string& str){
        return (unsigned)szHash(str) % P + 1;
    }
    
    /*
     @func:Realloc_insert
     @params: 重新插入字符串item，字符串处在ValueTable位置pos
     @对新的哈希桶进行T元素的插入，在重分配策略中使用
     */
    bool Realloc_insert(const std::string& item, unsigned pos) {
        
        // CuckooMap：只要找到一个桶的key对应槽位有空缺，则成功插入，否则执行随机踢出操作
        for (int i=0;i<NUM_BUCKETS;++i){
            if (insertToTable(item, i)) {
                PT[i]++;
                return true;
            }
        }
        
        // pointer of Item x
        unsigned int ptr = pos;
        unsigned int current_bucket = chooseTable(NUM_BUCKETS);
        unsigned int slot_index = chooseTable(NUM_SLOTS);
        std::string str(item);
        
        // kick-out policy
        for (int i = 0;i < ITER_ROUNDS_MAX;++i){
            unsigned key = HashF(ValueTable[ptr], current_bucket);
            unsigned array_index = NUM_SLOTS * (BUCKET_SIZE * current_bucket + key) + slot_index;
            // 将目标移出的对象尝试插入
            if (insertSpecificItemToTable(ValueTable[ptr], ptr, current_bucket)){
                PT[current_bucket]++;
                return true;
            }
            
            std::swap(ptr, T[array_index]);
            current_bucket = chooseTable(NUM_BUCKETS);
            slot_index = chooseTable(NUM_SLOTS);
        }
        // 如果失败，则考虑对桶进行扩容策略或增加hash函数桶
        insert_failure++;
        TriggerResize(1);
        return false;
    }

    // 踢出策略，选择目标PT表内值最小的元素
    unsigned int chooseWithDegree(unsigned ptr){
        // 有1/3概率选择随机跳出
        if (chooseTable(3)==0) return chooseTable(NUM_BUCKETS);
        // 剩下2/3概率选择当前最优值
        unsigned int min_degree = PT[0];
        unsigned int min_index = 0;
        for (int i=1;i<NUM_BUCKETS;++i){
            if (PT[i] < min_degree){
                min_degree = PT[i];
                min_index = i;
            }
        }
        return min_index;
    }
    
    /*
     @func:resize
     @重分配哈希函数
     */
    void resize(){
        // 增加哈希函数个数，同时改变桶的个数
        auto start = std::chrono::high_resolution_clock::now();
        ++NUM_BUCKETS;
        if (NUM_BUCKETS>=10) throw "Exception: NUM_PRIMES not enough.";
        // 删除旧序列
        delete[] T;
        memset(PT, 0, sizeof(PT));
        // 大小等于桶个数*桶大小*槽个数
        T = new unsigned int[(NUM_BUCKETS*BUCKET_SIZE)*NUM_SLOTS];
        
        // printf("====================New Hash==================\n");
        // printf("Params: Num_Buckets = %d; Bucket_Size = %d\n", NUM_BUCKETS,BUCKET_SIZE);
        // printf("==============================================\n\n");
        
        // Todo: Write Insert Algorithm
        for (int i=0; i < ValueTable.size();++i){
            // 注意：不可以简单地添加
            Realloc_insert(ValueTable[i], i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        // std::cout << "Time consumed by reconstruction : " << duration.count() * 1000.0f <<" ms"<< std::endl;
        time_consumed_for_reconstruction += duration.count() * 1000.0f;
        
    }
    
    /*
     @func:TriggerResize
     @触发重做Hash的策略
     */
    void TriggerResize(int op){
        if (op == 1) resize();
    }
    
    /*
     @func:chooseTable
     @随机选择一个哈希表进行替换
     */
    int chooseTable(int mod) {
        return distribution_alloc(rng) % mod;
    }

    // 插入元素到指定的哈希表，如果成功插入返回TRUE，插入失败返回FALSE
    bool insertToTable(const std::string& item, int table_index) {
        // 取得元素指向的指针
        unsigned int get_vector_pointer = (unsigned int)ValueTable.size() - 1;
        unsigned key = HashF(item, table_index);
        // T[NUM_BUCKETS][BUCKET_SIZE][NUM_SLOTS]
        unsigned array_index = NUM_SLOTS * (BUCKET_SIZE * table_index + key);
        // 尝试所有的槽位，如果都满则失败退出
        for (int offset = 0; offset < NUM_SLOTS; ++offset){
            if (!T[array_index + offset]) {
                T[array_index + offset] = get_vector_pointer;
                return true;
            }
        }
        return false;
    }
    
    // 插入指定元素到指定的哈希表，如果成功插入返回TRUE，插入失败返回FALSE
    bool insertSpecificItemToTable(const std::string& item, unsigned ptr, int table_index) {
        // 检查 ptr 是否越界
        if (ptr >= ValueTable.size()) {
            std::cout << "Error: ptr is out of bounds." << std::endl;
            throw "false";
            // return false;
        }
        // 取得元素指向的指针
        unsigned int get_vector_pointer = ptr;
        unsigned key = HashF(item, table_index);
        // T[NUM_BUCKETS][BUCKET_SIZE][NUM_SLOTS]
        unsigned array_index = NUM_SLOTS * (BUCKET_SIZE * table_index + key);
        // 尝试所有的槽位，如果都满则失败退出
        for (int offset = 0; offset < NUM_SLOTS; ++offset){
            if (!T[array_index + offset]) {
                T[array_index + offset] = get_vector_pointer;
                return true;
            }
        }
        return false;
    }


public:
    CuckooMap() : rng(std::random_device()()), distribution_alloc(0, 1) {
        // 值表首址需要填充空串
        ValueTable.push_back("");
        T = new unsigned int[(NUM_BUCKETS*BUCKET_SIZE)*NUM_SLOTS];
        PT = new unsigned int[HASH_MAXN];
        NUM_BUCKETS = 2; // 桶的数量，即哈希函数的个数
        BUCKET_SIZE = 125; // 每个桶的大小
        NUM_SLOTS = 16; // 槽位个数
    
        // 结果输出
        insert_failure = 0;
        insert_total = 0;
        time_consumed_for_reconstruction = 0.0;
    }

    ~CuckooMap(){
        delete[] T;
        delete[] PT;
        // delete[] ValueTable;
    }

    /*
     @func:insert
     @params: 重新插入字符串item
     @进行T元素的插入
     */
    bool insert(const std::string& item) {
        insert_total++;
        // insert to ValueTable
        ValueTable.push_back(item);
        
        // CuckooMap：只要找到一个桶的key对应槽位有空缺，则成功插入，否则执行随机踢出操作
        for (int i=0;i<NUM_BUCKETS;++i){
            if (insertToTable(item, i)) {
                PT[i]++;
                return true;
            }
        }
        
        // pointer of Item x
        unsigned int ptr = (unsigned int)ValueTable.size() - 1;
        // 策略1：随机寻找桶
        unsigned int current_bucket = chooseTable(NUM_BUCKETS);
        // 策略2：一定概率使用随机分配策略，另外一定概率选取最优策略
        // unsigned int current_bucket = chooseWithDegree(ptr);

        unsigned int slot_index = chooseTable(NUM_SLOTS);
        std::string str(item);
        
        // kick-out policy
        for (int i = 0;i < ITER_ROUNDS_MAX;++i){
            // pick a random table to kick out
            // pick a random slot to kick out
            unsigned key = HashF(ValueTable[ptr], current_bucket);
            unsigned array_index = NUM_SLOTS * (BUCKET_SIZE * current_bucket + key) + slot_index;
            // 将目标移出的对象尝试插入
            if (insertSpecificItemToTable(ValueTable[ptr], ptr, current_bucket)){
                PT[current_bucket]++;
                return true;
            }
            
            std::swap(ptr, T[array_index]);
            current_bucket = chooseTable(NUM_BUCKETS);
            slot_index = chooseTable(NUM_SLOTS);
        }
        // 如果失败，则考虑对桶进行扩容策略或增加hash函数桶
        insert_failure++;
        TriggerResize(1);
        return false;
    }

    /*
     @func:query
     @params: 字符串item
     @检查对象是否存在
     */
    bool query(const std::string& item) {
        for (int table_index = 0;table_index < NUM_BUCKETS; ++table_index){
            unsigned key = HashF(item, table_index);
            for (int offset = 0; offset < NUM_SLOTS; ++offset){
                unsigned array_index = NUM_SLOTS * (BUCKET_SIZE * table_index + key) + offset;
                if (ValueTable[T[array_index]]==item) return true;
            }
        }
        return false;
    }
    
    /*
     @func:get_insert_failure
     @返回插入失败次数
     */
    int get_insert_failure(){
        return insert_failure;
    }
    
    /*
     @func:get_insert_total
     @返回插入总次数
     */
    int get_insert_total(){
        return insert_total;
    }
    
    /*
     @func:get_time_consumed_for_reconstruction
     @返回重建总时间
     */
    float get_time_consumed_for_reconstruction(){
        return time_consumed_for_reconstruction;
    }

};

/*
 @func: generate_random_string
 @params: 字符串长度
 @用于生成随机测试字符串
 */
std::string generate_random_string(int length) {
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);
 
    for (int i = 0; i < length; ++i) {
        result += charset[dis(gen)];
    }
 
    return result;
}
// 测试对象集合
std::vector<std::string> test_string;

// 测试对象集合生成函数
void generate_test_example(int num){
    for (int i = 0;i<num;++i){
        test_string.push_back(generate_random_string(32));
    }
}


int main() {

    // {
    //     CuckooMap filter;
    
    //     generate_test_example(2000);
        
    //     auto start = std::chrono::high_resolution_clock::now();
    //     for (auto str:test_string){
    //         filter.insert(str);
    //     }
    //     auto end = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<float> duration = end - start;
    //     std::cout << "Time consumed Total : " << duration.count() * 1000.0f<<" ms"<< std::endl;
    //     std::cout << "#Insert_Total: "<<filter.get_insert_total();
    //     std::cout << " #Insert_Failure: "<<filter.get_insert_failure();
    //     std::cout << " #Time_Consumed_by_Reconstruction: "<<filter.get_time_consumed_for_reconstruction()<<" ms"<<std::endl;
    // }
    CuckooMap filter;
    filter.insert("apple");
    filter.insert("orange");
    filter.insert("banana");
    filter.insert("grape");
    std::cout << std::boolalpha;
    std::cout << "Have apple? " << filter.query("apple") << std::endl;
    std::cout << "Have grape? " << filter.query("grape") << std::endl;
    std::cout << "Have pineapple? " << filter.query("pineapple") << std::endl;
    
    // generate_test_example(2000);
    
    // auto start = std::chrono::high_resolution_clock::now();
    // for (auto str:test_string){
    //     filter.insert(str);
    // }
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<float> duration = end - start;
    // std::cout << "Time consumed Total : " << duration.count() * 1000.0f - filter.get_time_consumed_for_reconstruction()<<" ms"<< std::endl;
    // std::cout << "#Insert_Total: "<<filter.get_insert_total();
    // std::cout << " #Insert_Failure: "<<filter.get_insert_failure();
    // std::cout << " #Time_Consumed_by_Reconstruction: "<<filter.get_time_consumed_for_reconstruction()<<" ms"<<std::endl;
//    std::cout << std::boolalpha;
//    std::cout << "Contains apple? " << filter.contains("apple") << std::endl;
//    std::cout << "Contains grape? " << filter.contains("orange") << std::endl;

    return 0;
}
