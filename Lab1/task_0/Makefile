all : task0

task0 : count-words.o
	gcc -g -m32 -Wall -o task0 count-words.o

count-words.o : count-words.c
	gcc -g -m32 -Wall -c -o count-words.o count-words.c

.PHONY : clean
clean :
	rm -f *.o task0