all : hexeditplus task4

hexeditplus : hexeditplus.o
	gcc -g -m32 -Wall -o hexeditplus hexeditplus.o

task4 : task4.o
	gcc -g -m32 -Wall -o task4 task4.o

task4.o : task4.c
	gcc -g -Wall -m32 -fno-pie -fno-stack-protector -c -o task4.o task4.c

hexeditplus.o : hexeditplus.c
	gcc -g -m32 -Wall -c -o hexeditplus.o hexeditplus.c

.PHONY : clean
clean :
	rm -f *.o hexeditplus task4