all : task1

task1 : util.o start.o
	gcc -g -m32 -Wall -o task1 util.o start.o

util.o : util.c
	gcc -g -m32 -Wall -c -o util.o util.c

start.o : start.s
	nasm -g -f elf -w+all -o start.o start.s

.PHONY : clean
clean :
	rm -f *.o task1