#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "memDev.h"
#include "msgDev.h"

MODULE_LICENSE("GPL");

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
    return 0;
}

module_init(memMsgDev_init);

void memMsgdev_exit(void)
{
    int iRes;
    iRes = memDev_exit();
    printk(KERN_DEBUG "[memMsgDev]memMsgDev exit.\n");
}
module_exit(memMsgdev_exit);


