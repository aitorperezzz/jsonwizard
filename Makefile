# Compiler
CC=clang

# Compiler flags
CFLAGS=-g -Wall -Werror -std=c11 -fsanitize=address
CFLAGS_TEST=$(CFLAGS) -I./src
LDFLAGS_TEST=-lcmocka -fsanitize=address

# List sources, objects and dependencies
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%.c,src/%.o,$(SOURCES))
DEPENDS=$(patsubst src/%.c,src/%.d,$(SOURCES))

# Lists for tests
SOURCES_TEST=$(wildcard test/*.c)
OBJECTS_TEST=$(patsubst test/%.c,test/%,$(SOURCES_TEST))

# Compilation targets
TARGET=jsonwizard

.phony: all clean test

all: $(TARGET)

test: $(OBJECTS) $(OBJECTS_TEST)
	cd test ; \
	./test_string_type ; \
	./test_iterator ; \
	./test_vector

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

src/%.o: src/%.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

test/%: test/%.c Makefile
	$(CC) $(CFLAGS_TEST) $(LDFLAGS_TEST) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS) $(OBJECTS_TEST)
