.SILENT: link compile clean run
.PHONY: all link compile clean run

all: clean assemble compile link

link:
	ld -m elf_i386 start.o main.o util.o -o task0

compile:
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

assemble:
	nasm -f elf32 start.s -o start.o

clean:
	rm -f *.o task0

run:
	task0

task0b:
	nasm -f elf32 start.s -o start.o
	nasm -f elf32 hello.s -o hello.o
	ld -m elf_i386 start.o hello.o -o hello