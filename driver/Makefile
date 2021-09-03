obj-m += memMsgDev.o
memMsgDev-objs := memMsgDev_main.o memDev.o  msgDev.o ../pub/msg_pool.o

KERNEL_DIR=~/extend/qemu/stable_kernel_for_qemu/linux-stable-5.1/

all:
	-rm ../pub/msg_pool.o
	-rm ../pub/.msg_pool.o.cmd
	make -C $(KERNEL_DIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
	mkdir -p ../out/driver
	cp *.ko  ../out/driver

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean 2>/dev/null
	-rm  *.o 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null	
	-rm  *.ko 2>/dev/null
	-rm  *.symvers 2>/dev/null
	-rm  *.order 2>/dev/null
	-rm ../pub/msg_pool.o
	-rm ../pub/.msg_pool.o.cmd
	-rm ../out/driver/*
