all : task0

task0 : util.o task0.o
	gcc -g -m32 -Wall -no-pie -o task0 task0.o

task0.o : task0.c
	gcc -g -m32 -Wall -c -o task0.o task0.c

.PHONY : clean
clean :
	rm -f *.o task0