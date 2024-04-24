#include<stdio.h>

#include"config.h"
#include"functions.h"

int table[NUM], next[NUM];
int main()
{
    FILE *file = fopen("data.txt", "r"); // 打开文件
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }

    memset(next, -1, sizeof(next));
    memset(key, -1, sizeof(key));
    for(int i=0;i<NUM;i++)
        fscanf(file, "%d", &table[i]); // 从文件中读取数据
    printf("%d\n\n",table[0]);
    fclose(file); // 关闭文件
    return 0;
}