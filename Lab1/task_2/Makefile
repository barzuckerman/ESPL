all : task2

task0 : base.o
	gcc -g -m32 -Wall -o task2 base.o

base.o : base.c
	gcc -g -m32 -Wall -c -o base.o base.c

.PHONY : clean
clean :
	rm -f *.o task2