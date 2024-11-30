all : virusDetector

virusDetector : virusDetector.o
	gcc -g -m32 -Wall -o virusDetector virusDetector.o

virusDetector.o : virusDetector.c
	gcc -g -m32 -Wall -c -o virusDetector.o virusDetector.c

.PHONY : clean
clean :
	rm -f *.o virusDetector