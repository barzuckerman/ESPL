all : test

test : test.o
	gcc -g -m32 -Wall -no-pie -o test test.o

test.o : test.s
	nasm -g -f elf -w+all -o test.o test.s

.PHONY : clean
clean :
	rm -f *.o test