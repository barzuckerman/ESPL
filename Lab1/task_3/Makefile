all : task3

task3 : menu.o
	gcc -g -m32 -Wall -o task3 menu.o

menu.o : menu.c
	gcc -g -m32 -Wall -c -o menu.o menu.c

.PHONY : clean
clean :
	rm -f *.o task3