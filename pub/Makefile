all: msg_pool_test

msg_pool_test: msg_pool.o msg_pool_test.o
	gcc msg_pool.o msg_pool_test.o  -o  msg_pool_test

msg_pool.o : msg_pool.c
	gcc  -g -c msg_pool.c -o  msg_pool.o

msg_pool_test.o: msg_pool_test.c
	gcc  -g -c msg_pool_test.c -o  msg_pool_test.o

clean:
	-rm *.o 2>/dev/null
	-rm msg_pool_test 2>/dev/null
	-rm msg_pool.dwo 2>/dev/null
