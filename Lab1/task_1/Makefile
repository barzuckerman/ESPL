all : task1

task0 : adresses.o
	gcc -g -m32 -Wall -o task1 adresses.o

adresses.o : adresses.c
	gcc -g -m32 -Wall -c -o adresses.o adresses.c

.PHONY : clean
clean :
	rm -f *.o task1