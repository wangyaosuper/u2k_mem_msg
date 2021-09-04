
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/mm.h> 
#include <asm/io.h>
#include <linux/slab.h> 
#include <linux/kernel.h> 
#include <linux/workqueue.h>

#include "msgDev.h"
#include "memDev.h"
#include "../pub/msg_pool.h"

struct workqueue_struct *p_queue = NULL;

EvtNotify msg_k2u_evt_notify;
/* ----------------------------msg Dev init --------------------------- */

unsigned char * msgDevInit(unsigned char *pMemStruct, EvtNotify evt_notify){
    unsigned char *res;
    printk(KERN_DEBUG "msgDevInit : we will alloc a MsgPool with size of [%d] .\n", memGetMemStructSize());
    res = memStructInit_Server((MsgPoolPrint)printk, (MsgPoolMalloc)vmalloc, (MsgPoolFree)vfree, pMemStruct);
    if (NULL == res){
        printk(KERN_DEBUG "Error: msgDevInit() msgPoolInit_Alloc failed!\n");
        return res;
    }
    p_queue = create_singlethread_workqueue("msgDev_WorkQueue");
    if (NULL == p_queue){
        printk(KERN_DEBUG "Error: msgDevInit() create_workqueue failed!\n");
        kfree(res);
        return NULL;
    }
    msg_k2u_evt_notify = evt_notify;
    return res;
}

/* ---------------------------msg Evt process ------------------------- */
void msg_Evt_Work(struct work_struct *p){
    unsigned char *pData;
    int len;
    unsigned char *pBuf;
    printk(KERN_DEBUG "msg_Evt_Work() : Enter . \n");
            printk(KERN_EMERG "**** test Printk level EMERG.\n");
            printk(KERN_ALERT "**** test Printk level ALERT.\n");
            printk(KERN_CRIT "**** test Printk level CRIT.\n");
            printk(KERN_ERR "**** test Printk level ERR.\n");
            printk(KERN_WARNING "**** test Printk level WARNING.\n");
            printk(KERN_NOTICE "**** test Printk level NOITICE.\n");
            printk(KERN_INFO "**** test Printk level INFO.\n");
            printk(KERN_DEBUG "**** test Printk level DEBUG.\n");
    while(1){
        len = memGetMsg(&pData, E_U2K);
        if (-1 == len){
            printk(KERN_DEBUG "Info: msg_Evt_Work() the msg queue is empty.\n");
            return;
        }
        else if (0 > len){
            printk(KERN_DEBUG "Error: msg_Evt_Work() get the msg failed, return value[%d]. \n", len);
            return;
        }
        printk(KERN_DEBUG "SUCCESS: msg_Evt_work() get a msg from queue : %s\n", pData);
        pBuf = vmalloc(len + 100);
        if(NULL == pBuf){
            printk(KERN_DEBUG "Error: msg_evt_work() vmalloc buf failed, len[%d]!\n", len + 100);
            return;
        }
        strcpy(pBuf, "u2k echo : ");
        strcpy(pBuf + strlen(pBuf), pData);
        msg_send_msg(pBuf, strlen(pBuf) + 1);
        vfree(pBuf);
    }
}



int msg_Evt_Process(){
    struct work_struct *my_work;
    my_work = kmalloc(sizeof(struct work_struct), GFP_ATOMIC);
    if (NULL == my_work){
        printk(KERN_DEBUG "msg_Evt_Process() : kmalloc my_wokr failed . \n");
        return -1;
    }
    printk(KERN_DEBUG "msg_Evt_Process() : Enter . \n");
    INIT_WORK(my_work, msg_Evt_Work);
    queue_work(p_queue, my_work);
    return 0;
}

/* ---------------------- send msg ---------------------------------- */
int msg_send_msg(unsigned char *pData, unsigned int len){
    int res;
    printk(KERN_DEBUG "msg_send_msg() : Enter, jiffies64=%lld . \n", get_jiffies_64());
    res = memAddMsg(pData, len, E_K2U);
    if (0 > res){
        printk("Error : msg_send_msg() call memAddMsg failed , return value[%d]! \n", res);
        return res;
    }
    msg_k2u_evt_notify();
    return 0;
}

/* ------------------------get memory size needed -------------------- */
unsigned int msg_get_mem_struct_size(){
    return memGetMemStructSize();
}
