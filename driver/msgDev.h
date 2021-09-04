#ifndef _MSG_DEV_H_
#define _MSG_DEV_H_
#include <linux/mm.h> 

typedef int (*EvtNotify)(void);

unsigned char * msgDevInit(unsigned char *pMsgPool, EvtNotify evt_notify);

int msg_mem_mmap(struct file *filp, struct vm_area_struct *vma, unsigned char *mem_msg_buf);

int msg_Evt_Process(void);

unsigned int msg_get_mem_struct_size(void);

int msg_send_msg(unsigned char *pData, unsigned int len);


#endif
