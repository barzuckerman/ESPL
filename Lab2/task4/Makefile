all: mypipe

myshell: mypipe.o
	gcc -g -m32 -Wall -o mypipe mypipe.o

myshell.o: mypipe.c
	gcc -g -m32 -Wall -c -o mypipe.o mypipe.c

.PHONY: clean
clean:
	rm -f *.o mypipe
