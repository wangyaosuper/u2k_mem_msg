#include <stdio.h>
#include "forkscript.h"
#include "mem_usr.h"
#include "msg_usr.h"

#define MAX_BUF_SIZE 100

int testInstallCharDevDriver (void * pData){
    char buffer[MAX_BUF_SIZE + 1];
    forkScript(buffer, 
               MAX_BUF_SIZE + 1, 
               "script/installdev.py",
               "installdev.py", 
               "../demo/memKernelShareToUsr/driver/memMsgDev.ko", 
               "memMsgDev", 
               NULL);
    return 0;
}

int testUnInstallCharDevDriver (void * pData){
    char buffer[MAX_BUF_SIZE + 1];
    forkScript(buffer, 
               MAX_BUF_SIZE + 1, 
               "script/uninstalldev.py",
               "uninstalldev.py", 
               "../demo/memKernelShareToUsr/driver/memMsgDev.ko", 
               "memMsgDev", 
               NULL);
    return 0;
}

int testReadBuf(void * pData){
    char *buf = getMemMsgBuf();
    if (NULL == buf){
        printf("Error : testReadBuf() can't get the buf address. \n");
        return -1;
    }
    printf("read from mem buf : %s", buf);
    return 0;
}

int testWriteBuf(void * pData){
    char *buf = getMemMsgBuf();
    if (NULL == buf){
        printf("Error : testReadBuf() can't get the buf address. \n");
        return -1;
    }
    sprintf(buf, "hello buffer!\n");
    return 0;
}


int testCloseBuf(void * pData){
    closeMemMsgBuf();
    return 0;
}

int testInitMsgPool(void * pData){
    int res;
    printf("testInitMsgPool : Enter  , and we will mmap a memory for size[%d]. \n", usrGetMsgPoolMemorySize());
    res = memMsgBufInit(usrGetMsgPoolMemorySize(), usrEvtProcess);
    if (0 > res){
        printf("Error : testInitMsgPool() fail to memMsgBufInit, the return value[%d]!\n", res);
        return res;
    }
    res = usrMsgPoolInit(getMemMsgBuf(), memMsgIoctl);                    
    if (0 > res){
        printf("Error : testInitMsgPool() fail to usrMsgPoolInit, the return value[%d]!\n", res);
        return res;
    }
    return 0;
}

int testSendMsg(void * pData){
    char data[] = "hello this is a test.\n";
    usrSendMsg(data, sizeof(data));
    return 0;
}

