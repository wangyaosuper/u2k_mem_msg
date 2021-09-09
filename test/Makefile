GCC=arm-linux-gnueabi-gcc

all: testMsgDev york_benchmark
	mkdir -p ../out/test
	cp ./testMsgDev ../out/test/testMsgDev
	cp ./york_benchmark ../out/test/york_benchmark

testMsgDev: prepare main.o ../out/usr/libMemMsg.so.1
	$(GCC)   main.o -L../out/usr/ -lMemMsg -o testMsgDev

york_benchmark: prepare york_benchmark.o ../out/usr/libMemMsg.so.1
	$(GCC)   york_benchmark.o -L../out/usr/ -lMemMsg -o york_benchmark

main.o: main.c
	$(GCC) -g -c main.c -o  main.o

york_benchmark.o: york_benchmark.c
	$(GCC) -g -c york_benchmark.c -o  york_benchmark.o

prepare:
	-rm ../pub/msg_pool.o 2>/dev/null

clean:
	-rm *.o 2>/dev/null
	-rm testMsgDev 2>/dev/null
	-rm york_benchmark 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null


