all : hexeditplus task4

hexeditplus : task1.o
	gcc -g -m32 -Wall -o hexeditplus task1.o

task1.o : task1.c
	gcc -g -m32 -Wall -c -o task1.o task1.c

task4 : task4.o
	gcc -m32 -fno-pie -fno-stack-protector -no-pie -o task4 task4.c

task4.o : task4.c
	gcc -g -m32 -Wall -c -o task4.o task4.c

.PHONY : clean
clean :
	rm -f *.o hexeditplus