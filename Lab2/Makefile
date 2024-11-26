all : lab2

lab2 : LineParser.o myshell.o
	gcc -g -m32 -Wall -o lab2 LineParser.o myshell.o

LineParser.o : LineParser.c 
	gcc -g -m32 -Wall -c -o LineParser.o LineParser.c

myshell.o : myshell.c 
	gcc -g -m32 -Wall -c -o myshell.o myshell.c

.PHONY : clean
clean :
	rm -f *.o lab2