#include <sys/mman.h>
#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "mem_usr.h"

void *g_pBuffer = NULL;
int g_fd = -1;

Mem_Evt_Process mem_evt_process;

static void select_thread(void *data);

/* ----------------------------- no need basicly ------------------------- */
static void * getMemMsgBufPyhAddr(){
    int f;
    char stAddr[100];
    int len;
    void *pBuffer = NULL;
    if (NULL != g_pBuffer){
        return g_pBuffer;
    }

    f = open("/dev/memMsgDev", O_RDWR);
    if (0 == f){
        printf("Error : getMemMsgBuf() open /dev/memMsgDev failed , errno = %d \n", errno);
        return NULL;
    }
    len = read(f, stAddr, 100);
    if (len < 0){
        printf("Error : getMemMsgBuf() read the address from /dev/memMsgDev failed , errno = %d \n", errno);
        return NULL;
    }
    else if (0 == len){
        printf("Error : getMemMsgBuf() read the address from /dev/memMsgDev failed , len =0 \n");
        return NULL;
    }
    
    printf("getMemMsgBuf() get the address from /dev/memMsgDev successfully, stAddr = %s", stAddr);
    len = sscanf(stAddr, "%llu", &pBuffer);



    return pBuffer;
}

/* --------------------- init --------------------------------*/
int memMsgBufInit(unsigned int size ,Mem_Evt_Process process){
    static pthread_t pth_t;
    unsigned char * ret;
    int res;
    if (NULL != g_pBuffer){
        return 0;
    }

    if (g_fd < 0){
        g_fd = open("/dev/memMsgDev", O_RDWR);
        if (0 > g_fd){
            printf("Error : getMemMsgBuf() open /dev/memMsgDev failed , errno = %d \n", errno);
            return -1;
        }
    }

    g_pBuffer = mmap(NULL, size,PROT_READ|PROT_WRITE,MAP_SHARED, g_fd,0 );
    if( g_pBuffer == MAP_FAILED ) {
        printf("Error : getMemMsgBuf() fail mmap, errno[%d]!\n", errno);
        return -2;
    }
    printf("mmap() successfully. MagicNum=%s \n", g_pBuffer);
    mem_evt_process = process;
    

    res = pthread_create(&pth_t, NULL, select_thread, NULL);
    if (0 > res){
        printf("Error : memMsgBufInit() call pthread_create failed, return value[%d] , errno[%d]! \n", res, errno);
        return res;
    }
    return 0;
}


void * getMemMsgBuf(){
    return g_pBuffer;
}


/* ----------------------- close and tear down --------------------*/
int closeMemMsgBuf(){
    munmap(g_pBuffer, 1024*4);
    g_pBuffer = NULL;
    close(g_fd);
    g_fd = -1;
}

/* --------------- ioctl --------------------- */
int memMsgIoctl(unsigned int cmd , unsigned long data){
    printf("memMsgIoctl() : Enter. \n");
    return ioctl(g_fd, cmd , data);
}


/* ------------------- select ----------------------- */
static void select_thread(void *data){
    fd_set rfds;
    int retval;
    printf("select_thread() Enter. \n");
    while(1){
        FD_ZERO(&rfds);
        FD_SET(g_fd, &rfds);
        
        retval = select(g_fd + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1)
            printf("Error: select_thread() call select failed! \n");
        else if (retval){
            mem_evt_process();
        }
        else{
            printf("Error: select_thread() select no data! \n");
        }
    }
    

}
