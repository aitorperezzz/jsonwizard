# The compiler.
CC = gcc

# Compiler flags:
CFLAGS = -g -Wall

# Folders to find the include files.
INC = -Iinc

# Source files used.
SRC = src/wizard.c src/set.c src/print.c src/load.c src/free.c

# Name for the program.
TARGET = wizard

# Phony targets, not files.
.phony: clean

# Build target executable:
all:
	$(CC) -o $(TARGET) $(SRC) $(INC)
	mv $(TARGET) bin/

# Clean the directory where the binary is stored.
clean:
	-rm bin/$(TARGET)
