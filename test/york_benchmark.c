#include <stdio.h>
#include "../pub/msg_pool.h"

int testInitMemStruct(void * pData);

int main()
{
    unsigned long long jiffies64_begin, jiffies64_end, i, n=3;
    int ret = testInitMemStruct(NULL);
    if (0 != ret){
        printf("testInitMsgPool failed, ret value = %d \n", ret);
        return ret;
    }
    jiffies64_begin = getJiffies64();
    for(i=0; i<=100000000; ++i){
        n = n * n;
    }
    jiffies64_end = getJiffies64();
    printf("begin = %llu, end = %llu, span = %llu. \n",
           jiffies64_begin,
           jiffies64_end,
           jiffies64_end - jiffies64_begin);
    printf("n = %llu .\n", n);
    return 0;
}
