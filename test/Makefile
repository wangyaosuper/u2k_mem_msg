GCC=arm-linux-gnueabi-gcc

all: testMsgDev
	mkdir -p ../out/test
	cp ./testMsgDev ../out/test/testMsgDev

testMsgDev: prepare main.o ../out/usr/libMemMsg.so.1
	$(GCC)   main.o -L/home/wyao/extend/qemu/realtime_experiment/mem_and_msg/out/usr/ -lMemMsg -o testMsgDev

main.o: main.c
	$(GCC) -g -c main.c -o  main.o

prepare:
	-rm ../pub/msg_pool.o 2>/dev/null

clean:
	-rm *.o 2>/dev/null
	-rm rm testMsgDev 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null

