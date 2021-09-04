#ifndef _MSG_POOL_H_
#define _MSG_POOL_H_


typedef void (*MsgPoolPrint)(char *);
typedef void* (*MsgPoolMalloc)(unsigned int);
typedef void (*MsgPoolFree)(void *);


typedef enum MSG_DIRECTION{
    E_K2U,
    E_U2K
} EN_MSG_DIRECTION;


unsigned char * memStructInit(MsgPoolPrint pPrintFunc, 
                            MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc,
                            unsigned char *pMemStruct);
unsigned char * memStructInit_Server(MsgPoolPrint pPrintFunc, 
                                  MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc,
                                  unsigned char *pMemStruct);


int memAddMsg(unsigned char *pData, unsigned int len, EN_MSG_DIRECTION direct);
int memGetMsg(unsigned char **pData, EN_MSG_DIRECTION direct);
unsigned int memGetQueueLen(EN_MSG_DIRECTION d);
int memGetFreeCubNum(EN_MSG_DIRECTION d);
unsigned int memGetMemStructSize(void);
unsigned char * getMsgPoolAddr(unsigned char * pMemStruct);

void setJiffies64(unsigned long long jiffies64);
unsigned long long getJiffies64(void);

#endif




