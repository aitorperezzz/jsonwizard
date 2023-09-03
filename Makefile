# Compiler
CC := clang

# Compiler flags
CFLAGS := -g -Wall -Werror -std=c11 -fsanitize=address
TEST_CFLAGS := $(CFLAGS) -I./src
TEST_LDFLAGS := -lcmocka -fsanitize=address

# List sources, objects and dependencies
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,src/%.o,$(SOURCES))
DEPENDS := $(patsubst src/%.c,src/%.d,$(SOURCES))
TARGET := jsonwizard

# Lists for tests
TEST_SOURCES := test/test_main.c $(SOURCES)
TEST_SOURCES := $(filter-out src/wizard.c, $(TEST_SOURCES))
TEST_OBJECTS := $(patsubst %.c,%.o,$(TEST_SOURCES))
TEST_DEPENDS := $(patsubst %.c,%.d,$(TEST_SOURCES))
TEST_TARGET := testjsonwizard

.phony: all clean test

all: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

-include $(TEST_DEPENDS)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_CFLAGS) $(TEST_LDFLAGS) $(TEST_OBJECTS) -o $(TEST_TARGET)

src/%.o: src/%.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

test/%.o: test/%.c Makefile
	$(CC) $(TEST_CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS) $(TEST_OBJECTS) $(TEST_TARGET) $(TEST_DEPENDS)
