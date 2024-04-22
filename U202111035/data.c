#include<time.h>
#include<stdio.h>
#include<stdlib.h>
int main(){
    FILE *fp = fopen("data.txt", "w");
    srand(time(0));
    for(int i=0;i<1000;i++){
        int num=rand();
        fprintf(fp, "%d\n", num);
    }
    fclose(fp);
    return 0;
}