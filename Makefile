# Compiler
CC=gcc

# Compiler flags
CFLAGS=-g -Wall -Werror -std=c11

# List sources, objects and dependencies
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
DEPENDS=$(patsubst %.c,%.d,$(SOURCES))

# Name of the executable
TARGET=jsonwizard

.phony: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

-include $(DEPENDS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS)
