SDL=`pkg-config --cflags sdl2`
TTF=`pkg-config --cflags `
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb $(SDL) $(LIBS)
CC=gcc
LIBS=`pkg-config --libs SDL2_ttf` -lm

ked: main.c
	$(CC) $(CFLAGS) -o ked main.c
