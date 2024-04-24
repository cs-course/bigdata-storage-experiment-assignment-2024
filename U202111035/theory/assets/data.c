#include<time.h>
#include<stdio.h>
#include<stdlib.h>

#include"config.h"

int main(){
    FILE *fp = fopen("data.txt", "w");
    srand(time(0));
    for(int i=0;i<NUM;i++){
        int num=rand()%LIMIT;
        fprintf(fp, "%d\n", num);
    }
    fclose(fp);
    return 0;
}