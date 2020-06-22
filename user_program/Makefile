all:
	gcc master.c -o master -fsanitize=undefined,address
	gcc slave.c -o slave -fsanitize=undefined,address
clean:
	rm slave master
