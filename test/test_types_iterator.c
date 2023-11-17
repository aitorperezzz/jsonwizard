#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "types/types_iterator.h"

static void test_types_iterator_create(void **state)
{
    Iterator iterator = types_iterator_create((void *)0x25689, 16);
    assert_ptr_equal(iterator.pointer, 0x25689);
    assert_int_equal(iterator.size, 16);
    iterator = types_iterator_create(NULL, 16);
    assert_ptr_equal(iterator.pointer, NULL);
    assert_int_equal(iterator.size, 16);
}

static void test_types_iterator_invalid(void **state)
{
    Iterator iterator = types_iterator_invalid();
    assert_ptr_equal(iterator.pointer, NULL);
    assert_int_equal(iterator.size, 0);
}

static void test_types_iterator_copy(void **state)
{
    // Create an array with data to copy from
    int origin[3];
    for (int i = 0; i < 3; i++)
    {
        origin[i] = i;
    }
    // Create a destination array and initialise with zeros
    int destination[3];
    memset(destination, '\0', 3 * sizeof(int));
    // Copy and check
    Iterator first = types_iterator_create(origin, sizeof(int));
    Iterator last = types_iterator_increase(first, 3);
    Iterator result = types_iterator_create(destination, sizeof(int));
    Iterator output = types_iterator_copy(first, last, result);
    // Check the output iterator has the correct values
    assert_ptr_equal(output.pointer, destination + 3);
    assert_int_equal(output.size, sizeof(int));
    // Check the copy process has been correct
    assert_memory_equal(origin, destination, 3 * sizeof(int));

    // Try to copy to a result in the middle of the first and last iterators
    result = types_iterator_create(origin + 1, sizeof(int));
    output = types_iterator_copy(first, last, result);
    assert_ptr_equal(output.pointer, NULL);
    assert_int_equal(output.size, 0);

    // Provide inconsistent iterators
    last = types_iterator_create(origin + 3, sizeof(char *));
    result = types_iterator_create(destination, sizeof(int));
    output = types_iterator_copy(first, last, result);
    assert_ptr_equal(output.pointer, NULL);
    assert_int_equal(output.size, 0);
}

static void test_types_iterator_find(void **state)
{
    // Piece of memory where the data will be searched
    int origin[3];
    for (int i = 0; i < 3; i++)
    {
        origin[i] = i;
    }
    // Data to find
    int toFind = 1;
    // Find and check the result
    Iterator first = types_iterator_create(origin, sizeof(int));
    Iterator last = types_iterator_increase(first, 3);
    Iterator output = types_iterator_find(first, last, &toFind);
    assert_ptr_equal(output.pointer, origin + 1);
    assert_int_equal(output.size, sizeof(int));

    // Search in a memory area where the value is not found
    for (int i = 0; i < 3; i++)
    {
        origin[i] = i + 6;
    }
    // Find and check the result
    first = types_iterator_create(origin, sizeof(int));
    last = types_iterator_increase(first, 3);
    output = types_iterator_find(first, last, &toFind);
    assert_ptr_equal(output.pointer, origin + 3);
    assert_int_equal(output.size, sizeof(int));

    // Provide NULL data
    output = types_iterator_find(first, last, NULL);
    assert_ptr_equal(output.pointer, NULL);
    assert_int_equal(output.size, 0);

    // Provide inconsistent iterators
    last = types_iterator_create(origin + 3, sizeof(char *));
    output = types_iterator_find(first, last, &toFind);
    assert_ptr_equal(output.pointer, NULL);
    assert_int_equal(output.size, 0);
}

static void test_types_iterator_equal(void **state)
{
    Iterator iterator1 = types_iterator_create((void *)0x1, 32);
    Iterator iterator2 = types_iterator_create((void *)0x1, 32);
    assert_true(types_iterator_equal(iterator1, iterator2));
    iterator2 = types_iterator_create((void *)0x2, 32);
    assert_false(types_iterator_equal(iterator1, iterator2));
    iterator2 = types_iterator_create((void *)0x1, 16);
    assert_false(types_iterator_equal(iterator1, iterator2));
}

static void test_types_iterator_get(void **state)
{
    Iterator iterator = types_iterator_create((void *)0x15689, 32);
    assert_ptr_equal(types_iterator_get(iterator), 0x15689);
    iterator = types_iterator_create(NULL, 32);
    assert_ptr_equal(types_iterator_get(iterator), NULL);
}

static void test_types_iterator_distance(void **state)
{
    size_t distance;

    // Valid iterators
    Iterator iterator1 = types_iterator_create((void *)0x15689, sizeof(int));
    Iterator iterator2 = types_iterator_create((void *)0x15689 + 4 * sizeof(int), sizeof(int));
    assert_int_equal(types_iterator_distance(iterator1, iterator2, &distance), CODE_OK);
    assert_int_equal(distance, 4);

    // Invalid iterators
    iterator2 = types_iterator_create((void *)0x15689, sizeof(char *));
    assert_int_equal(types_iterator_distance(iterator1, iterator2, &distance), CODE_LOGIC_ERROR);

    // Invalid pointer for output data
    iterator2 = types_iterator_create((void *)0x15689 + 4 * sizeof(int), sizeof(int));
    assert_int_equal(types_iterator_distance(iterator1, iterator2, NULL), CODE_MEMORY_ERROR);
}

static void test_types_iterator_increase(void **state)
{
    Iterator iterator = types_iterator_create((void *)0x1, sizeof(int));
    Iterator result = types_iterator_increase(iterator, 1);
    assert_ptr_equal(result.pointer, 0x1 + sizeof(int));
    assert_int_equal(result.size, sizeof(int));
    result = types_iterator_increase(iterator, 0);
    assert_ptr_equal(result.pointer, 0x1);
    assert_int_equal(result.size, sizeof(int));
}

static void test_types_iterator_decrease(void **state)
{
    Iterator iterator = types_iterator_create((void *)0x1, sizeof(int));
    Iterator result = types_iterator_decrease(iterator, 1);
    assert_ptr_equal(result.pointer, 0x1 - sizeof(int));
    assert_int_equal(result.size, sizeof(int));
    result = types_iterator_decrease(iterator, 0);
    assert_ptr_equal(result.pointer, 0x1);
    assert_int_equal(result.size, sizeof(int));
}