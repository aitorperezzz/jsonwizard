# The compiler.
CC = gcc

# Compiler flags:
CFLAGS = -g -Wall

# Folders to find the include files.
INC = -Iinc

# Folder where the output of the compilation will be.
OUT = bin

# Source files used.
SRC = src/wizard.c src/set.c src/print.c src/load.c src/free.c

# Name for the program.
TARGET = wizard

# Phony targets, not files.
.phony: clean directories

# Build target executable:
all: directories
	$(CC) -o $(TARGET) $(SRC) $(INC) $(CFLAGS)
	mv $(TARGET) $(OUT)

# Clean the directory where the program is stored.
clean:
	-rm $(OUT)/wizard

# Create the bin directory if it does not exist already.
directories:
	mkdir -p ${OUT}
