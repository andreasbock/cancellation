all:
	gcc -c -o cancellation.o cancellation.c 
	gcc cancellation.o -o cancellation

run:
	./cancellation
