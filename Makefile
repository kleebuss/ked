SDL=`pkg-config --cflags sdl2`
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb $(SDL) $(LIBS)
CC=gcc
LIBS=`pkg-config --libs sdl2` -lm

ked: main.c
	$(CC) $(CFLAGS) -o ked main.c
