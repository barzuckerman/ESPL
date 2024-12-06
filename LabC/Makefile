all: myshell Looper mypipeline

myshell: LineParser.o myshell.o
	gcc -g -m32 -Wall -o myshell LineParser.o myshell.o

Looper: Looper.o
	gcc -g -m32 -Wall -o Looper Looper.o

mypipeline: mypipeline.o
	gcc -g -m32 -Wall -o mypipeline mypipeline.o

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

Looper.o: Looper.c
	gcc -g -m32 -Wall -c -o Looper.o Looper.c

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

mypipeline.o: mypipeline.c
	gcc -g -m32 -Wall -c -o mypipeline.o mypipeline.c

.PHONY: clean
clean:
	rm -f *.o myshell Looper mypipeline