CC = gcc
STD = c99
OUT = executable
LIBS = -lncurses -lm

all:
	$(CC) -std=$(STD) *.c -o $(OUT) $(LIBS)
