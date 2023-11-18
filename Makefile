emu: read_inodes.c creation.c navigation.c funcs.h
	gcc -Wall -pedantic -std=c99 read_inodes.c creation.c navigation.c -g -o emu

clean:
	rm -f emu