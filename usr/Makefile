GCC=arm-linux-gnueabi-gcc

all: libMemMsg.so.1

libMemMsg.so.1: prepare memMsg_main.o forkscript.o mem_usr.o msg_usr.o 
	$(GCC) -fPIC -g -c ../pub/msg_pool.c -o ../pub/msg_pool.o
	$(GCC) -shared memMsg_main.o forkscript.o mem_usr.o msg_usr.o ../pub/msg_pool.o -lpthread -o  libMemMsg.so.1
	mkdir -p ../out/usr
	cp libMemMsg.so.1 ../out/usr/libMemMsg.so.1
	cp libMemMsg.so.1 ../out/usr/libMemMsg.so

prepare:
	-rm ../pub/msg_pool.o 2>/dev/null

msg_usr.o:
	$(GCC) -fPIC -g -c msg_usr.c -o msg_usr.o


memMsg_main.o : memMsg_main.c
	$(GCC) -fPIC -g -c memMsg_main.c -o  memMsg_main.o

mem_usr.o : mem_usr.c
	$(GCC) -fPIC -g -c mem_usr.c -o  mem_usr.o

forkscript.o : forkscript.c
	$(GCC) -fPIC -g -c forkscript.c -o  forkscript.o

clean:
	-rm *.o 2>/dev/null
	-rm libMemMsg.so.1 2>/dev/null
	-rm libMemMsg.so 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null

