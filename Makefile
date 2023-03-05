# Compiler
CC=clang

# Compiler flags
CFLAGS=-g -Wall -Werror -std=c11 -fsanitize=address
CFLAGS_TEST=$(CFLAGS) -I./src -I/opt/homebrew/include/
LDFLAGS_TEST=-lcmocka -L/opt/homebrew/lib -fsanitize=address

# List sources, objects and dependencies
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%.c,src/%.o,$(SOURCES))
DEPENDS=$(patsubst src/%.c,src/%.d,$(SOURCES))

# Lists for tests
SOURCES_TEST=$(wildcard test/*.c)
OBJECTS_TEST=$(patsubst test/%.c,test/%,$(SOURCES_TEST))
TARGETS_TEST=$(patsubst test/%.o,test/%,$(OBJECTS_TEST))

# Compilation targets
TARGET=jsonwizard

.phony: all clean test

all: $(TARGET)

test: $(TARGETS_TEST)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

-include $(DEPENDS)

src/%.o: src/%.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

test/%.o: test/%.c Makefile
	$(CC) $(CFLAGS_TEST) -MMD -MP -c $< -o $@

test/%: test/%.o Makefile
	$(CC) $< -o $@ $(LDFLAGS_TEST)

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS) $(TARGETS_TEST)
