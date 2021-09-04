#include <stdio.h>
#include <stdlib.h>
#include "msg_pool.h"
#include "mem_struct.h"


/*  ---------- ops --------------- */
typedef enum OP_TYPE{
    OP_ADD,
    OP_GET
} EN_OP_TYPE;

typedef enum CH_TYPE{
    CH_SAME,
    CH_INC
} EN_CH_TYPE;


struct Operation{
    EN_OP_TYPE opType;
    EN_CH_TYPE chType;
    unsigned char data;
    unsigned int len;
};




struct Operation ops[] = 
    {{OP_ADD, CH_SAME, 'a', 16},
     {OP_GET, CH_SAME, 0, 0},
     {OP_ADD, CH_SAME, 'b', 1024},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_ADD, CH_SAME, 'b', 1024},
     {OP_ADD, CH_INC, 'c', 1024},
     {OP_GET, CH_SAME, 0, 0},
     {OP_ADD, CH_SAME, 'd', 1024},
     {OP_ADD, CH_SAME, 'b', 1025},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_ADD, CH_SAME, 'b', 7000},
     {OP_ADD, CH_INC, 'e', 5024},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0},
     {OP_GET, CH_SAME, 0, 0}
    };


/* ---------- opstack ----------- */

struct OpQueue {
    int  begin;
    int  end;
    struct Operation *stack[10000000];
};

struct OpQueue opQueue;

static int opQueueInit(){
    opQueue.begin = -1;
    opQueue.end = -1;
    return 0;
}

static int addOp(struct Operation * o){
    ++opQueue.begin;
    opQueue.stack[opQueue.begin] = o;
    return 0;
}

static struct Operation * getOp(){
    if (opQueue.begin <= opQueue.end){
        return NULL;
    }
    ++opQueue.end;
    return opQueue.stack[opQueue.end];
}

/* ------------------ ut operation ---------------------- */
#define MP_ASSERT(A, B, C...)  while(1){         \
                          if (!(A)){\
                          printf(B, ##C);\
                          abort();\
                          }\
        break;\
    }
    

int testAddAndGet(){
    unsigned char *pData;
    unsigned int i, n;
    int len;
    int res;
    EN_MSG_DIRECTION d;
    struct Operation *pGetOp;
    pData = malloc(memGetMemStructSize());
    msgPoolInit_Server(printf, malloc, free, pData);
    opQueueInit();

    for (d = E_K2U; d <= E_U2K; ++d){
        for (i = 0 ; i < sizeof(ops)/sizeof(struct Operation); ++i){
            if (OP_ADD == ops[i].opType){
                pData = malloc(ops[i].len);
                for(n = 0 ; n < ops[i].len; ++n){
                    switch (ops[i].chType){
                    case CH_SAME:
                        pData[n] = ops[i].data;
                        break;
                    case CH_INC:
                        pData[n] = ops[i].data + n;
                        break;
                    }
                }
                res = memAddMsg(pData, ops[i].len, d);
                MP_ASSERT(0 == res, "ERROR func[%s]: OP[%d] , memAddMsg failed! res value[%d]\n", __FUNCTION__, i ,res);
                free(pData);
                addOp(&ops[i]);
            }
            else{
                pGetOp = getOp();
                if (NULL != pGetOp){
                    len = memGetMsg(&pData, d);
                    MP_ASSERT(len == pGetOp->len, "ERROR func[%s]: OP[%d] , memGetMsg() failed! return value[%d] \n", __FUNCTION__, i, len);
                    for (n = 0; n < len ; ++n){
                        switch(pGetOp->chType){
                        case CH_SAME:
                            MP_ASSERT(pData[n] == pGetOp->data, 
                                      "ERROR func[%s]: OP[%d] , pData[%d] = %d , not equal to [%d]",
                                      __FUNCTION__, i, n , pData[n], pGetOp->data);
                            break;
                        case CH_INC:
                            MP_ASSERT(pData[n] == (unsigned char)(pGetOp->data + n), 
                                      "ERROR func[%s]: OP[%d] , pData[%d] = %d , not equal to [%d]",
                                      __FUNCTION__, i, n , pData[n], pGetOp->data + n);
                            break;
                        }
                    }
                }
                else{
                    len = memGetMsg(&pData, d);
                    MP_ASSERT(-1 == len, 
                              "ERROR func[%s]: OP[%d] , get memGetMsg when no msg in Queue, and the return Value[%d] is not -1! \n",
                              __FUNCTION__, n, len);
                }
            }
            printf("SUCC func[%s]: OP[%d] , A OP TEST SUCCESS, DIRECT[%d] , OP_TYPE[%d], QueueLen[%d], PoolFree[%d] .\n",
                   __FUNCTION__, i, d, ops[i].opType, memGetQueueLen(d), memGetFreeCubNum(d));
        }
    }
    return 0;
}








int main(){
    testAddAndGet();    
    return 0;
}
