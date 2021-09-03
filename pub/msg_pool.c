
#include "msg_pool.inc"
#include "msg_pool.h"

#ifndef NULL
#define NULL 0
#endif

MsgPoolPrint  msgPoolPrint = NULL;
MsgPoolMalloc msgPoolMalloc = NULL;
MsgPoolFree   msgPoolFree  = NULL;


struct MsgPool *msgPool;
unsigned char *tmp_buffer;
/* --------------------- static func for memcpy ------------------- */
static char* itoa(int num,char* str,int radix)
{
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
	unsigned unum;
	int i=0,j,k;
 
	
	if(radix==10&&num<0)
	{
		unum=(unsigned)-num;
		str[i++]='-';
	}
	else unum=(unsigned)num;
 
	
	do
	{
		str[i++]=index[unum%(unsigned)radix];
		unum/=radix;
 
	}while(unum);
 
	str[i]='\0';
 
	
	if(str[0]=='-') k=1;
	else k=0;
 
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];
		str[j]=str[i-1+k-j];
		str[i-1+k-j]=temp;
	}
 
	return str;
}


static void mem_memcpy(void * a , void * b, unsigned int size){
    unsigned char *x = a;
    unsigned char *y = b;
    unsigned int i;
    char str[10];
    msgPoolPrint("enter mem_memcpy().\n");
    for (i = 0 ; i < size ; ++i){
        x[i] = y[i];
        itoa(i, str, 10);
        /*
        msgPoolPrint("mem_memcpy() i=");
        msgPoolPrint(str);
        msgPoolPrint("\n");
        */
    }
    msgPoolPrint("leave mem_memcpy().\n");
}

static void mem_strcpy(char * a , char * b){
    while('\0' != *b){
        *a = *b;
        ++a;
        ++b;
    }
    *a = '\0';
}

static void mem_memset(void * a , unsigned char k, unsigned int size){
    unsigned char *x = a;
    unsigned int i;
    for (i = 0 ; i < size ; ++i){
        x[i] = k;
    }
}



/* ---------------------- static func for heap --------------------- */
static int allocAFreeCub(struct MsgCub * pHeap){
    unsigned int i;
    for (i = 0 ; i < MAX_MSG_QUEUE_LEN; ++i){
        if (0 == pHeap[i].isUsed){
            pHeap[i].isUsed = 1;
            return i;
        }
    }
    return -1;
}

static int freeCubs(struct MsgCub *heap, int cub, unsigned char *pData, unsigned int len){
    unsigned int tmpCub;
    while(0 <= cub){
        if (0 == heap[cub].isUsed){
            heap[cub].isUsed = 0;
            cub = heap[cub].nextCub;
            heap[cub].nextCub = -1;
            return -3;
        }
        if (NULL == pData){
            heap[cub].isUsed = 0;
            cub = heap[cub].nextCub;
            heap[cub].nextCub = -1;
            continue;
        }
        if (heap[cub].nextCub >= 0){
            if (len <= MSG_CUB_DATA_LEN){
                heap[cub].isUsed = 0;
                cub = heap[cub].nextCub;
                heap[cub].nextCub = -1;
                return -1;
            }
            mem_memcpy((void*)pData, (void*)heap[cub].data, MSG_CUB_DATA_LEN);
            len -= MSG_CUB_DATA_LEN;
            pData += MSG_CUB_DATA_LEN;
        }
        else{
            if (len > MSG_CUB_DATA_LEN){
                heap[cub].isUsed = 0;
                cub = heap[cub].nextCub;
                heap[cub].nextCub = -1;
                return -2;
            }
            mem_memcpy(pData,  heap[cub].data, len);
            len -= len;
            pData += len;
        }
        heap[cub].isUsed = 0;
        tmpCub = cub;
        cub = heap[cub].nextCub;
        heap[tmpCub].nextCub = -1;
    }
    return 0;
}

static int allocFreeCubs(struct MsgCub *heap, unsigned char *pData, unsigned int len){
    unsigned int count = len / MSG_CUB_DATA_LEN;
    unsigned int i;
    int cub;
    int lastCub = -1;
    int head = -1;
    if (len % MSG_CUB_DATA_LEN != 0){
        ++count;
    }
    for (i = 0 ; i < count; ++i){
        msgPoolPrint("before allocAFreeCub() in allocFreeCubs(). \n");
        cub = allocAFreeCub(heap);
        msgPoolPrint("after allocAFreeCub() in allocFreeCubs(). \n");
        if (-1 == cub){
            freeCubs(heap, head, NULL, 0);
            head = -1;
            return head;
        }
        if (-1 == head){
            head = cub;
        }
        if (-1 != lastCub){
            heap[lastCub].nextCub = cub;
            msgPoolPrint("heap[lastCub].nextCub = cub.\n");
        }
        lastCub = cub;
        heap[cub].nextCub = -1;
        msgPoolPrint("before mem_memcpy in allocFreeCubs().\n");
        if (i != count - 1){
            mem_memcpy(heap[cub].data,  pData, MSG_CUB_DATA_LEN);
            pData += MSG_CUB_DATA_LEN;
        } else {
            mem_memcpy(heap[cub].data,  pData, len % MSG_CUB_DATA_LEN);
            pData += len % MSG_CUB_DATA_LEN;
        }
        msgPoolPrint("after mem_memcpy in allocFreeCubs().\n");
    }
    return head;
}


static struct DirectionMsgPool * getDirectionMsgPool(EN_MSG_DIRECTION d){
    switch (d){
    case E_K2U:
        return &msgPool->k2uPool;
    case E_U2K:
        return &msgPool->u2kPool;
    defualt:
        return NULL;
    }
    return NULL;
}


/* -------------------init func for msgPool -----------------------*/
unsigned char * msgPoolInit_Server(MsgPoolPrint pPrintFunc, 
                                   MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc, 
                                   unsigned char *pool){
    unsigned int i;
    struct MsgPool * msgPool = (struct MsgPool *) msgPoolInit(pPrintFunc, 
                                       pMallocFunc, pFreeFunc,
                                       pool);
    msgPoolPrint("msgPoolInit() finished successfully. \n");
    mem_strcpy(msgPool->magicNum, "This is a msg Pool between Kernel and usr mode.\n");
    msgPool->k2uPool.begin = -1;
    msgPool->k2uPool.end = -1;
    msgPool->u2kPool.begin = -1;
    msgPool->u2kPool.end = -1;
    msgPoolPrint("msgPool u2kPool begin end init successfully. \n");
    mem_memset(msgPool->u2kPool.queue, 0x0, sizeof(msgPool->u2kPool.queue));
    mem_memset(msgPool->k2uPool.queue, 0x0, sizeof(msgPool->k2uPool.queue));
    mem_memset(msgPool->k2uPool.heap, 0x0, sizeof(msgPool->k2uPool.heap));
    mem_memset(msgPool->u2kPool.heap, 0x0, sizeof(msgPool->u2kPool.heap));
    msgPoolPrint("msgPool u2kPool queue heep clean successfully. \n");
    for (i = 0 ; i < MAX_MSG_POOL_SIZE; ++i){
        msgPool->k2uPool.heap[i].nextCub = -1;
        msgPool->u2kPool.heap[i].nextCub = -1;
    }
    msgPoolPrint("msgPool u2kPool queue heep init successfully. \n");
    return pool;
}

unsigned char * msgPoolInit(MsgPoolPrint pPrintFunc, 
                            MsgPoolMalloc pMallocFunc, MsgPoolFree pFreeFunc, 
                            unsigned char *pMsgPool){
    msgPoolPrint = pPrintFunc;
    msgPoolMalloc = pMallocFunc;
    msgPoolFree = pFreeFunc;
    msgPool = (struct MsgPool*)pMsgPool;

    if (NULL == msgPool){
        return NULL;
    }
    msgPoolPrint("Leave msgPoolInit(). \n");
    return pMsgPool;
}




/* -------------------------------- The API for Queue --------------------------------- */

int memAddMsg(unsigned char *pData, unsigned int len, EN_MSG_DIRECTION direct){
    int head;
    msgPoolPrint("Enter memAddMsg().\n");
    struct DirectionMsgPool * pDMP = getDirectionMsgPool(direct);
    msgPoolPrint("getDirectionMsgPool() successfully.\n");
    unsigned int next = pDMP->begin + 1;
    if (next >= MAX_MSG_QUEUE_LEN){
        next = 0;
    }
    msgPoolPrint("pDMP->begin set successfully.\n");
    if (next == pDMP->end){
        return -1;
    }
    msgPoolPrint("next != pDMP->end.\n");
    head = allocFreeCubs(pDMP->heap, pData, len);
    if (0 > head){
        return head - 10;
    }
    msgPoolPrint("allocFreeCubs() successfully.\n");
    pDMP->queue[next].msgCubNum = head;
    pDMP->queue[next].datalen = len;
    pDMP->begin = next;
    return 0;
}

int memGetMsg(unsigned char **ppData,  EN_MSG_DIRECTION direct){
    int res;
    struct DirectionMsgPool * pDMP = getDirectionMsgPool(direct);
    unsigned int next;

    if (pDMP->begin == pDMP->end){
        return -1;
    }
    next = pDMP->end + 1;
    if (next >= MAX_MSG_QUEUE_LEN){
        next = 0;
    }

    msgPoolFree(tmp_buffer);
    tmp_buffer = msgPoolMalloc(pDMP->queue[next].datalen);
    if (NULL == tmp_buffer){
        return -2;
    }
    res = freeCubs(pDMP->heap, pDMP->queue[next].msgCubNum, tmp_buffer, pDMP->queue[next].datalen);
    if (0 > res){
        msgPoolFree(tmp_buffer);
        return res - 10;
    }
    *ppData = tmp_buffer;
    pDMP->end = next;
    return pDMP->queue[next].datalen;
}

unsigned int memGetQueueLen(EN_MSG_DIRECTION d){
    struct DirectionMsgPool * pDMP = getDirectionMsgPool(d);
    if (pDMP->begin > pDMP->end){
        return pDMP->begin - pDMP->end;
    }
    else if(pDMP->begin < pDMP->end){
        return pDMP->begin + MAX_MSG_QUEUE_LEN - pDMP->end;
    }
    else{
        return 0;
    }
}

int memGetFreeCubNum(EN_MSG_DIRECTION d){
    struct DirectionMsgPool * pDMP = getDirectionMsgPool(d);
    unsigned int count = 0;
    unsigned int i;
    struct MsgCub *pHeap = pDMP->heap;
    for (i = 0 ; i < MAX_MSG_QUEUE_LEN; ++i){
        if (0 == pHeap[i].isUsed){
            ++ count;
        }
    }
    return count;
}


unsigned int memGetMsgPoolMemorySize(void){
    return sizeof(struct MsgPool);
}
