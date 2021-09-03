#include <stdio.h>

int testInitMsgPool(void * pData);
int testSendMsg(void * pData);


int main()
{
    int ret;
    char* p;
    p = testInitMsgPool(NULL);
    if (NULL != p){
        printf("testInitMsgPool failed, ret value = %d \n", ret);
        return ret;
    }
    printf("testInitMsgPool() finished succesfully!\n");
    ret = testSendMsg(NULL);
    if (0 != ret){
        printf("testSendMsg failed, ret value = %d \n", ret);
        return ret;
    }
    printf("testSendMsg() finished succesfully \n");
    return 0;
}
