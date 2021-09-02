all: libMemMsg.so.1

libMemMsg.so.1: prepare memMsg_main.o forkscript.o mem_usr.o msg_usr.o 
	gcc -fPIC -g -c ../pub/msg_pool.c -o ../pub/msg_pool.o
	gcc -shared -Wl,-h,libDriverTest.so.1.1.1  memMsg_main.o forkscript.o mem_usr.o msg_usr.o ../pub/msg_pool.o -lpthread -o  libMemMsg.so.1
	cp libMemMsg.so.1 ../../../../bin/demo/memKernelShareToUsr/usr/libMemMsg.so.1

prepare:
	-rm ../pub/msg_pool.o 2>/dev/null

msg_usr.o:
	gcc -fPIC -g -c msg_usr.c -o msg_usr.o


memMsg_main.o : memMsg_main.c
	gcc -fPIC -g -c memMsg_main.c -o  memMsg_main.o

mem_usr.o : mem_usr.c
	gcc -fPIC -g -c mem_usr.c -o  mem_usr.o

forkscript.o : forkscript.c
	gcc -fPIC -g -c forkscript.c -o  forkscript.o

clean:
	-rm *.o 2>/dev/null
	-rm libMemMsg.so.1 2>/dev/null
	-rm ../pub/msg_pool.o 2>/dev/null

