#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "msg_usr.h"
#include "../pub/msg_pool.h"


MsgIoctl msg_ioctl = NULL;

int usrMemStructInit(unsigned char *pMemStruct, MsgIoctl ioctlFunc){
    int res;
    msg_ioctl = ioctlFunc;
    printf("before msgPoolInit() successfully. MagicNum=%s \n", pMemStruct);
    char *p;
    p = memStructInit(printf, malloc , free,  pMemStruct);
    printf("msgPoolInit() successfully. MagicNum=%s \n", p);
    return 0;
}

int usrSendMsg(unsigned char *pData, unsigned int len){
    int res;
    res = memAddMsg(pData, len, E_U2K);
    if (0 > res){
        printf("Error: usrSendMsg() failed to memAddMsg, the return value[%d]", res);
        return res;
    }
    printf("memAddMsg() finished successfully.\n");
    res = msg_ioctl(0 , NULL);
    if (0 < res){
        printf("Error: usrSendMsg() failed to ioctl, the return value[%d], errno[%d]", res, errno);
    }
    return res;
}


/*  ---------------- get msg pool memory size ---------*/
unsigned int usrGetMemStructSize(){
    return memGetMemStructSize();
}


/* ---------------- evt process ----------------------*/
int usrEvtProcess(){
    unsigned char *pData;
    int len;
    unsigned char *pBuf;
    printf( "usrEvtProcess() : Enter . \n");
    while(1){
        len = memGetMsg(&pData, E_K2U);
        if (-1 == len){
            printf("Info: usrEvtProcess() the msg queue is empty.\n");
            return -1;
        }
        else if (0 > len){
            printf( "Error: usrEvtProcess() get the msg failed, return value[%d]. \n", len);
            return -2;
        }
        printf( "SUCCESS: usrEvtProcess() get a msg from queue : %s\n", pData);
    }
    return 0;
}
