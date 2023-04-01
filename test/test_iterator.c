#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "iterator.c"

static void test_create(void **state)
{
    Iterator iterator = iterator_create((void *)0x25689, 16);
    assert_ptr_equal(iterator.pointer, 0x25689);
    assert_int_equal(iterator.size, 16);
    iterator = iterator_create(NULL, 16);
    assert_ptr_equal(iterator.pointer, NULL);
    assert_int_equal(iterator.size, 16);
}

static void test_copy(void **state)
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
    Iterator first = iterator_create(origin, sizeof(int));
    Iterator last = iterator_increase(first, 3);
    Iterator result = iterator_create(destination, sizeof(int));
    Iterator output = iterator_copy(first, last, result);
    // Check the output iterator has the correct values
    assert_ptr_equal(output.pointer, destination + 3);
    assert_int_equal(output.size, sizeof(int));
    // Check the copy process has been correct
    assert_memory_equal(origin, destination, 3 * sizeof(int));

    // Try to copy to a result in the middle of the first and last iterators
    Iterator result2 = iterator_create(origin + 1, sizeof(int));
    Iterator output2 = iterator_copy(first, last, result2);
    assert_ptr_equal(output2.pointer, NULL);
    assert_int_equal(output2.size, 0);
}

static void test_find(void **state)
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
    Iterator first = iterator_create(origin, sizeof(int));
    Iterator last = iterator_increase(first, 3);
    Iterator output = iterator_find(first, last, &toFind);
    assert_ptr_equal(output.pointer, origin + 1);
    assert_int_equal(output.size, sizeof(int));

    // Search in a memory area where the value is not found
    int origin2[3];
    for (int i = 0; i < 3; i++)
    {
        origin[i] = i + 6;
    }
    // Find and check the result
    Iterator first2 = iterator_create(origin2, sizeof(int));
    Iterator last2 = iterator_increase(first2, 3);
    Iterator output2 = iterator_find(first2, last2, &toFind);
    assert_ptr_equal(output2.pointer, origin2 + 3);
    assert_int_equal(output2.size, sizeof(int));
}

static void test_equal(void **state)
{
    Iterator iterator1 = iterator_create((void *)0x1, 32);
    Iterator iterator2 = iterator_create((void *)0x1, 32);
    assert_true(iterator_equal(iterator1, iterator2));
    iterator2 = iterator_create((void *)0x2, 32);
    assert_false(iterator_equal(iterator1, iterator2));
    iterator2 = iterator_create((void *)0x1, 16);
    assert_false(iterator_equal(iterator1, iterator2));
}

static void test_get(void **state)
{
    Iterator iterator = iterator_create((void *)0x1, 32);
    assert_ptr_equal(iterator_get(iterator), 0x1);
    iterator = iterator_create(NULL, 32);
    assert_ptr_equal(iterator_get(iterator), NULL);
}

static void test_distance(void **state)
{
    Iterator iterator1 = iterator_create((void *)0x1, sizeof(int));
    Iterator iterator2 = iterator_create((void *)0x1 + 4 * sizeof(int), sizeof(int));
    assert_int_equal(iterator_distance(iterator1, iterator2), 4);
}

static void test_increase(void **state)
{
    Iterator iterator = iterator_create((void *)0x1, sizeof(int));
    Iterator result = iterator_increase(iterator, 1);
    assert_ptr_equal(result.pointer, 0x1 + sizeof(int));
    assert_int_equal(result.size, sizeof(int));
}

static void test_decrease(void **state)
{
    Iterator iterator = iterator_create((void *)0x1, sizeof(int));
    Iterator result = iterator_decrease(iterator, 1);
    assert_ptr_equal(result.pointer, 0x1 - sizeof(int));
    assert_int_equal(result.size, sizeof(int));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_copy),
        cmocka_unit_test(test_find),
        cmocka_unit_test(test_equal),
        cmocka_unit_test(test_get),
        cmocka_unit_test(test_distance),
        cmocka_unit_test(test_increase),
        cmocka_unit_test(test_decrease),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}