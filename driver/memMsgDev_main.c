#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "memDev.h"
#include "msgDev.h"
#include "../pub/msg_pool.h"

MODULE_LICENSE("GPL");

/* ------ Kernel Hook begin ------- */
typedef void (*DoTimerHook )(unsigned long long jiffies64);
/* Need Kernel EXPORT_SYMBOL(gFunc_DoTimerHook); Otherwise it can't be extern here*/
/* Need to init when dev init, and set to NULL when dev removed!!!!! */
extern DoTimerHook gFunc_DoTimerHook;
void doTimerHook(unsigned long long jiffies64);
/* ------ Kernel Hook end ------ */

int  memMsgDev_init(void)
{
    int iRes;
    unsigned char *pBuff;
    printk(KERN_DEBUG "Page Size = %u", PAGE_SIZE);
    printk(KERN_INFO "[memMsgDev]memMsgDev init.\n");
    /* only mem need dev to be install as a module, msg is a logic function on the mem dev */
    iRes = memDev_init(msg_get_mem_struct_size() , &pBuff, msg_Evt_Process); 
    if (0 > iRes){
        printk(KERN_DEBUG "Error: memMsgDev_init() fail to memDev_init, return value[%d]. \n", iRes);
        return -1;
    }
    pBuff = msgDevInit(pBuff, mem_wakeup_poll);
    if (NULL == pBuff){
        printk(KERN_DEBUG "Error: memMsgDev_init() fail to msgDev_init, return value[%d]. \n", iRes);
        return -1;
    }
    gFunc_DoTimerHook = doTimerHook;
    return 0;
}

module_init(memMsgDev_init);

void memMsgdev_exit(void)
{
    int iRes;
    iRes = memDev_exit();
    gFunc_DoTimerHook = NULL;
    printk(KERN_DEBUG "[memMsgDev]memMsgDev exit.\n");
}
module_exit(memMsgdev_exit);


/* ----- Kernel Hook begin ----- */
void doTimerHook(unsigned long long jiffies64){
    setJiffies64(jiffies64);
}
/* ----- Kernel Hook end ----- */
