all: all_driver all_usr all_test

all_driver:
	cd driver && make all

all_test: all_usr
	cd test && make all

all_usr:
	cd usr && make all

clean: clean_driver clean_usr clean_pub clean_test

clean_driver:
	-cd driver && make clean

clean_usr:
	-cd usr && make clean

clean_pub:
	-cd pub && make clean

clean_test:
	-cd test && make clean
