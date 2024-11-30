all : part0

part0 : Bubblesort.o
	gcc -g -m32 -Wall -o part0 Bubblesort.o

Bubblesort.o : Bubblesort.c
	gcc -g -m32 -Wall -c -o Bubblesort.o Bubblesort.c

.PHONY : clean
clean :
	rm -f *.o part0