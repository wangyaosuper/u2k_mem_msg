CFLAGS += "-Wno-error" 

obj-m += memMsgDev.o
memMsgDev-objs := memMsgDev_main.o memDev.o  msgDev.o ../pub/msg_pool.o

all:
	-rm ../pub/msg_pool.o
	-rm ../pub/.msg_pool.o.cmd
	make -Wno-error -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	cp *.ko  ../../../../bin/demo/memKernelShareToUsr/driver

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 2>/dev/null
	-rm  *.o 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null	
	-rm  *.ko 2>/dev/null
	-rm  *.symvers 2>/dev/null
	-rm  *.order 2>/dev/null
	-rm ../pub/msg_pool.o
	-rm ../pub/.msg_pool.o.cmd

