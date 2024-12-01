all : hexaPrint

hexaPrint : hexaPrint.o
	gcc -g -m32 -Wall -o hexaPrint hexaPrint.o

hexaPrint.o : hexaPrint.c
	gcc -g -m32 -Wall -c -o hexaPrint.o hexaPrint.c

.PHONY : clean
clean :
	rm -f *.o hexaPrint