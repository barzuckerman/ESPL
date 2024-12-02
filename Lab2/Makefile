all: myshell Looper mypipe

myshell: LineParser.o myshell.o
	gcc -g -m32 -Wall -o myshell LineParser.o myshell.o

Looper: Looper.o
	gcc -g -m32 -Wall -o Looper Looper.o

mypipe: mypipe.o
	gcc -g -m32 -Wall -o mypipe mypipe.o

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

Looper.o: Looper.c
	gcc -g -m32 -Wall -c -o Looper.o Looper.c

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

mypipe.o: mypipe.c
	gcc -g -m32 -Wall -c -o mypipe.o mypipe.c

.PHONY: clean
clean:
	rm -f *.o myshell Looper mypipe