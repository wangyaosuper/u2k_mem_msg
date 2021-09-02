all: all_driver all_usr

all_driver:
	-mkdir ../../../bin/demo/memKernelShareToUsr/driver
	cd driver && make all

all_usr:
	-mkdir ../../../bin/demo/memKernelShareToUsr/usr
	cd usr && make all

clean: clean_driver clean_usr clean_pub

clean_driver:
	-cd driver && make clean

clean_usr:
	-cd usr && make clean

clean_pub:
	-cd pub && make clean