#ifndef _USR_MSG_H_
#define _USR_MSG_H_

typedef int (*MsgIoctl)(unsigned int cmd , unsigned long data);

int usrMsgPoolInit(unsigned char *msgPool, MsgIoctl msg_ioctl);

int usrSendMsg(unsigned char *pData, unsigned int len);

unsigned int usrGetMemStructSize();

int usrEvtProcess();

#endif
