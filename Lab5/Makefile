all : my_loader

my_loader : my_loader.o
	gcc -m32 -c my_loader.c -o my_loader.o
	ld -o my_loader my_loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

my_loader.o : my_loader.c
	gcc -g -m32 -Wall -c -o my_loader.o my_loader.c

.PHONY : clean
clean :
	rm -f *.o my_loader