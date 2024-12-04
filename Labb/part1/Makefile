all : virusDetector

virusDetector : AntiVirus.o
	gcc -g -m32 -Wall -o virusDetector AntiVirus.o

AntiVirus.o : AntiVirus.c
	gcc -g -m32 -Wall -c -o AntiVirus.o AntiVirus.c

.PHONY : clean
clean :
	rm -f *.o virusDetector