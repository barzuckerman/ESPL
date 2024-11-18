all : task3

task3 : menu_map.o
	gcc -g -m32 -Wall -o task3 menu_map.o
menu_map.o : menu_map.c
	gcc -g -m32 -Wall -fno-pie -c -o menu_map.o menu_map.c

.PHONY : clean
clean :
	rm -f *.o task3