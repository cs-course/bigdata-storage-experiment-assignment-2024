#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define FILTER_SIZE 10000
#define NUM_DIMENSIONS 2
#define NUM_ELEMENTS 1000
#define TEST_QUERIES 1000

// 定义多维布隆过滤器结构
typedef struct {
    bool filter[FILTER_SIZE][NUM_DIMENSIONS];
} MDBF;

// 初始化多维布隆过滤器
void initMDBF(MDBF *mdbf) {
    for (int i = 0; i < FILTER_SIZE; i++) {
        for (int j = 0; j < NUM_DIMENSIONS; j++) {
            mdbf->filter[i][j] = false;
        }
    }
}

// 添加元素到多维布隆过滤器
void addToMDBF(MDBF *mdbf, int *element) {
    for (int i = 0; i < NUM_DIMENSIONS; i++) {
        int index = element[i] % FILTER_SIZE;
        mdbf->filter[index][i] = true;
    }
}

// 检查元素是否存在于多维布隆过滤器中
bool isInMDBF(MDBF *mdbf, int *element) {
    for (int i = 0; i < NUM_DIMENSIONS; i++) {
        int index = element[i] % FILTER_SIZE;
        if (!mdbf->filter[index][i]) {
            return false;
        }
    }
    return true;
}

// 检查元素是否为所存数据（确定）
bool isInMDBF_ofcourse(int element_repository[NUM_ELEMENTS][NUM_DIMENSIONS],int *element) {
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        for (int j = 0; j < NUM_DIMENSIONS; j++) {
            if(element[j] == element_repository[i][j])
                return true;
        }
    }
    return false;
}

int main() {
    // 初始化多维布隆过滤器
    MDBF mdbf;
    initMDBF(&mdbf);

    //保留所存数据
    int element[NUM_ELEMENTS][NUM_DIMENSIONS];

    // 添加一些元素
    srand(time(NULL));
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        for (int j = 0; j < NUM_DIMENSIONS; j++) {
            element[i][j] = rand(); // 随机生成元素
        }
        addToMDBF(&mdbf, element[i]);
    }

    // 测试查询
    int false_positives = 0;
    clock_t start_time, end_time;
    double total_time;

    start_time = clock(); // 记录开始时间

    for (int i = 0; i < TEST_QUERIES; i++) {
        int query[NUM_DIMENSIONS];
        for (int j = 0; j < NUM_DIMENSIONS; j++) {
            query[j] = rand() % FILTER_SIZE; // 随机生成查询
        }
        bool result = isInMDBF_ofcourse(element,query);
        if (!result) {
            // 检查是否为误报
            bool false_positive = isInMDBF(&mdbf,query);
            if (false_positive) {
                false_positives++;
            }
        }
    }

    end_time = clock(); // 记录结束时间
    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC; // 计算总时间

    // 打印结果
    printf("FILTER_SIZE & NUM_ELEMENTS & NUM_DIMENSIONS: %d & %d & %d\n", FILTER_SIZE,NUM_ELEMENTS,NUM_DIMENSIONS);
    printf("False positives: %d/%d\n", false_positives, TEST_QUERIES);
    printf("Total time taken: %.6f seconds\n", total_time);

    return 0;
}
