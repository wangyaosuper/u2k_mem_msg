#ifndef _MEM_STRUCT_INC_
#define _MEM_STRUCT_INC_
#include "msg_pool.inc"

struct MemStruct{
    char magicNum[64];
    unsigned long long jiffies64;
    struct MsgPool msg_pool;
};

#endif
