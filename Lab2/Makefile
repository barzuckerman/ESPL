all : myshell

myshell : LineParser.o Looper.o myshell.o
	gcc -g -m32 -Wall -o myshell LineParser.o Looper.o myshell.o

LineParser.o : LineParser.c 
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

Looper.o : Looper.c 
	gcc -g -m32 -Wall -c -o Looper.o Looper.c

myshell.o : myshell.c 
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

.PHONY : clean
clean :
	rm -f *.o myshell