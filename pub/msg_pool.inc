#ifndef _MSG_POOL_INC_
#define _MSG_POOL_INC_


#define MAX_MSG_QUEUE_LEN 100
#define MSG_CUB_DATA_LEN  1024
#define MAX_MSG_POOL_SIZE 300

struct MsgNode{
    int               msgCubNum;
    unsigned int      datalen;
};

struct MsgCub{
    char              isUsed;
    int               nextCub;
    unsigned char     data[MSG_CUB_DATA_LEN];
};


struct DirectionMsgPool{
    int               begin;
    int               end;
    struct MsgNode    queue[MAX_MSG_QUEUE_LEN];
    struct MsgCub     heap[MAX_MSG_POOL_SIZE];
};

struct MsgPool{
    char              magicNum[64];
    struct DirectionMsgPool  k2uPool;
    struct DirectionMsgPool  u2kPool;
};

#endif
