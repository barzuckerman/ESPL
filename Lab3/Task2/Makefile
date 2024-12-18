all : task2

task2 : util.o main.o start.o
	gcc -g -m32 -Wall -no-pie -o task2 util.o main.o start.o

util.o : util.c
	gcc -g -m32 -Wall -c -o util.o util.c

main.o : main.c
	gcc -g -m32 -Wall -c -o main.o main.c

start.o : start.s
	nasm -g -f elf -w+all -o start.o start.s

.PHONY : clean
clean :
	rm -f *.o task2