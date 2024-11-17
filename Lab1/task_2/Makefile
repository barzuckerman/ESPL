all : task3

task3 : base.o menu.o
	gcc -g -m32 -Wall -o task3 base.o menu.o

base.o : base.c
	gcc -g -m32 -Wall -c -o base.o base.c

menu.o : menu.c
	gcc -g -m32 -Wall -c -o menu.o menu.c

.PHONY : clean
clean :
	rm -f *.o task3