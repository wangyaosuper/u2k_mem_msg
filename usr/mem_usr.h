#ifndef _MEM_USR_H_
#define _MEM_USR_H_

typedef int (*Mem_Evt_Process)();

int memStructBufInit(unsigned int size, Mem_Evt_Process process);
int closeMemMsgBuf();

void * getMemMsgBuf();
int memMsgIoctl(unsigned int cmd , unsigned long data);

#endif
