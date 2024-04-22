#include <iostream>
using namespace std;
//一些全局变量
int m = 4;//最小哈希随机排列组合次数，即sig向量维数
int r = 2, b = 2;//分为两段，每段长度为2
int doc1[10], doc2[10], doc3[10], doc4[10];//源文档特征向量
int sig1[4], sig2[4], sig3[4], sig4[4];//最小哈希降维后的序列
int rand1[10], rand2[10], rand3[10], rand4[10];//随机排序序列
//桶结构体
typedef struct barrel {
    int B_sig[2];//该桶特征向量
    int doc_name[4] = { 0,0,0,0 };//初始为0，放入桶的文档对应位设为1
    bool if_use = false;//该桶是否使用
}B;
B b1[4], b2[4];//两个段对应的两个桶组，由于文档数为4，为方便处理，直接设桶组大小为4
//随机初始文档特征向量
void initdoc()
{
    for (int i = 0; i < 10; i++)
    {
        doc1[i] = rand() % 2;
        doc2[i] = rand() % 2;
        doc3[i] = rand() % 2;
        doc4[i] = rand() % 2;
    }
}
//随机初始组合序列
void initrand(int random[])
{
    int length = 0;
    while (1)
    {
        int k = rand() % 10 + 1;
        int flag = 0;
        for (int i = 0; i < length; i++)
        {
            if (random[i] == k)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1) continue;
        random[length++] = k;
        if (length == 10) break;
    }
}
//查看
void viewdoc(int doc[],int cnt)
{
    for (int j = 0; j < cnt; j++) printf("%d ", doc[j]);
    printf("\n");

}
//最小哈希降维处理
void to_signal(int doc[], int sig[])
{
    for (int i = 0; i < 10; i++) if (doc[rand1[i] - 1] == 1)
    {
        sig[0] = rand1[i];
        break;
    }
    for (int i = 0; i < 10; i++) if (doc[rand2[i] - 1] == 1)
    {
        sig[1] = rand2[i];
        break;
    }
    for (int i = 0; i < 10; i++) if (doc[rand3[i] - 1] == 1) 
    {
        sig[2] = rand3[i];
        break;
    }
    for (int i = 0; i < 10; i++) if (doc[rand4[i] - 1] == 1) 
    {
        sig[3] = rand4[i];
        break;
    }
}

void to_barrel(int sig[], B b1[], B b2[], int seq)
{
    //段1分桶
    for (int i = 0; i < 4; i++)
    {
        if (b1[i].B_sig[0] == sig[0] && b1[i].B_sig[1] == sig[1])
        {
            b1[i].doc_name[seq - 1] = 1;
            break;
        }
        if (b1[i].if_use == 1) continue;
        b1[i].B_sig[0] = sig[0];
        b1[i].B_sig[1] = sig[1];
        b1[i].doc_name[seq - 1] = 1;
        b1[i].if_use = 1;
        break;
    }
    //段2分桶
    for (int i = 0; i < 4; i++)
    {
        if (b2[i].B_sig[0] == sig[2] && b2[i].B_sig[1] == sig[3])
        {
            b2[i].doc_name[seq - 1] = 1;
            break;
        }
        if (b2[i].if_use == 1) continue;
        b2[i].B_sig[0] = sig[2];
        b2[i].B_sig[1] = sig[3];
        b2[i].doc_name[seq - 1] = 1;
        b2[i].if_use = 1;
        break;
    }
}

void show_barrel(B b1[], B b2[])
{
    printf("b1_content:\n");
    for (int i = 0; i < 4; i++)
    {
        if (b1[i].if_use == 1)
        {
            printf("sig=%d,%d  doc={%d,%d,%d,%d}\n",
                b1[i].B_sig[0], b1[i].B_sig[1],
                b1[i].doc_name[0], b1[i].doc_name[1],
                b1[i].doc_name[2], b1[i].doc_name[3]);
        }
    }
    printf("b2_content:\n");
    for (int i = 0; i < 4; i++)
    {
        if (b2[i].if_use == 1)
        {
            printf("sig=%d,%d  doc={%d,%d,%d,%d}\n",
                b2[i].B_sig[0], b2[i].B_sig[1],
                b2[i].doc_name[0], b2[i].doc_name[1],
                b2[i].doc_name[2], b2[i].doc_name[3]);
        }
    }
}

int main()
{
    //随机数据，最小哈希处理
    initdoc();
    initrand(rand1);initrand(rand2);initrand(rand3);initrand(rand4);
    to_signal(doc1, sig1); to_signal(doc2, sig2); to_signal(doc3, sig3); to_signal(doc4, sig4);
    //展示结果
    printf("doc is\n");
    viewdoc(doc1, 10); viewdoc(doc2, 10); viewdoc(doc3, 10); viewdoc(doc4, 10);
    printf("rand is\n");
    viewdoc(rand1, 10); viewdoc(rand2, 10); viewdoc(rand3, 10); viewdoc(rand4, 10);
    printf("sig is\n");
    viewdoc(sig1, 4); viewdoc(sig2, 4); viewdoc(sig3, 4); viewdoc(sig4, 4);
    //进行LSH处理
    //对sig向量进行分桶
    to_barrel(sig1, b1, b2, 1); to_barrel(sig2, b1, b2, 2);
    to_barrel(sig3, b1, b2, 3); to_barrel(sig4, b1, b2, 4);
    show_barrel(b1, b2);

    return 0;
}
