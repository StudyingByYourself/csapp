CC = /usr/bin/gcc
CFLAGS = -Wall -g -Werror -std=gnu99 -m64 -fPIC

EXECUTABLE = csapp
SRC = ./src
CODE = ./src/memory/instruction.c ./src/memory/dram.c \
		./src/disk/code.c ./src/cpu/mmu.c ./src/main.c

.PHONY: csapp
csapp:
	$(CC) $(CFLAGS) -I$(SRC) $(CODE) -o $(EXECUTABLE)

run:
	./$(EXECUTABLE)