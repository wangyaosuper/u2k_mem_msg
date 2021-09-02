#ifndef _MSG_POOL_H_
#define _MSG_POOL_H_


typedef void (*MsgPoolPrint)(char *);
typedef void* (*MsgPoolMalloc)(unsigned int);
typedef void (*MsgPoolFree)(void *);


typedef enum MSG_DIRECTION{
    E_K2U,
    E_U2K
} EN_MSG_DIRECTION;


unsigned char * msgPoolInit(MsgPoolPrint pPrintFunc, 
                            MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc,
                            unsigned char *msgPool);
unsigned char * msgPoolInit_Server(MsgPoolPrint pPrintFunc, 
                                  MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc,
                                  unsigned char *msgPool);


int memAddMsg(unsigned char *pData, unsigned int len, EN_MSG_DIRECTION direct);
int memGetMsg(unsigned char **pData, EN_MSG_DIRECTION direct);
unsigned int memGetQueueLen(EN_MSG_DIRECTION d);
int memGetFreeCubNum(EN_MSG_DIRECTION d);
unsigned int memGetMsgPoolMemorySize(void);


#endif




