#include<string.h>
#define OK 1
#define ERROR -1
extern int bucket[NUM],table[NUM];
int hash1(int key) {
    return key/CAPACITY;
}

int hash2(int key) {
    return key%CAPACITY;
}

int insert(int key) {
    int index1 = hash1(key);
    int index2 = hash2(key);
    int loop1 = loop(index1);
    int loop2 = loop(index2);
    if(bucket[index1]==-1) {
        bucket[index1] = key;
        table[index1]=index2;
        return OK;
    }
    if(bucket[index2]==-1) {
        bucket[index2] = key;
        table[index2]=index1;
        return OK;
    }
    /*发生循环的逻辑较为复杂*/
}

int delete(int key) {
    int index = search(key);
    if(index==ERROR) return ERROR;
    table[index] = -1;
    bucket[index] = -1;
    return OK;
}

int search(int key) {
    int index1 = hash1(key);
    int index2 = hash2(key);
    if(bucket[index1]==key) return index1;
    if(bucket[index2]==key) return index2;
    return ERROR;
}