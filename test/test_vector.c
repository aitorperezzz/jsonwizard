#include <stdio.h>
#include <assert.h>

#include "vector.h"

static ResultCode noFree(void *data)
{
    return CODE_OK;
}

static ResultCode myStringFree(void *data)
{
    if (data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    // The data pointed at is a char *
    char **string = (char **)data;
    if (*string != NULL)
    {
        free(*string);
    }
    return CODE_OK;
}

static Vector *createIntegerVector(void)
{
    Vector *vector = vector_create(sizeof(int), noFree);
    int values[10];
    for (size_t i = 0; i < 10; i++)
    {
        values[i] = i;
        assert_int_equal(vector_push(vector, &values[i]), CODE_OK);
    }
    return vector;
}

static Vector *createStringVector(void)
{
    Vector *vector = vector_create(sizeof(char *), myStringFree);
    char *value;
    for (size_t i = 0; i < 10; i++)
    {
        const size_t length = sizeof(char) + 1;
        value = malloc(length);
        snprintf(value, length, "%c", (char)(i + '0'));
        assert_int_equal(vector_push(vector, &value), CODE_OK);
    }
    return vector;
}

static void test_vector_create(void **state)
{
    Vector *vector = vector_create(sizeof(int), noFree);
    assert_ptr_equal(vector->data, NULL);
    assert_int_equal(vector->size, 0);
    assert_int_equal(vector->capacity, 0);
    assert_int_equal(vector->elementSize, sizeof(int));
    assert(vector->freeCallback == noFree);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = vector_create(sizeof(int), NULL);
    assert_ptr_equal(vector, NULL);
}

static void test_vector_size(void **state)
{
    Vector *vector = createIntegerVector();
    assert_int_equal(vector_size(vector), vector->size);
    assert_int_equal(vector_size(vector), 10);
    int number = 400;
    assert_int_equal(vector_push(vector, &number), CODE_OK);
    assert_int_equal(vector_size(vector), vector->size);
    assert_int_equal(vector_size(vector), 11);
    assert_int_equal(vector_clear(vector), CODE_OK);
    assert_int_equal(vector_size(vector), vector->size);
    assert_int_equal(vector_size(vector), 0);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_size(vector), 0);
}

static void test_vector_push(void **state)
{
    Vector *vector = vector_create(sizeof(int), noFree);
    int number = 4;
    assert_int_equal(vector_push(vector, &number), CODE_OK);
    assert_ptr_not_equal(vector->data, NULL);
    assert_memory_equal(vector->data, &number, sizeof(int));
    assert_int_equal(vector->size, 1);
    assert_int_equal(vector->capacity, 1);
    assert_int_equal(vector->elementSize, sizeof(int));
    int number2 = 78;
    assert_int_equal(vector_push(vector, &number2), CODE_OK);
    assert_ptr_not_equal(vector->data, NULL);
    assert_memory_equal(vector->data, &number, sizeof(int));
    assert_memory_equal(vector->data + sizeof(int), &number2, sizeof(int));
    assert_int_equal(vector->size, 2);
    assert_int_equal(vector->capacity, 2);
    assert_int_equal(vector->elementSize, sizeof(int));
    assert_int_equal(vector_push(vector, NULL), CODE_MEMORY_ERROR);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_push(vector, &number2), CODE_MEMORY_ERROR);
}

static void test_vector_clear(void **state)
{
    Vector *vector = createIntegerVector();
    assert_int_equal(vector_clear(vector), CODE_OK);
    assert_int_equal(vector->size, 0);
    assert_int_equal(vector->capacity, 10);
    assert_int_equal(vector->elementSize, sizeof(int));
    assert_int_equal(vector_clear(vector), CODE_OK);
    assert_int_equal(vector->size, 0);
    assert_int_equal(vector->capacity, 10);
    assert_int_equal(vector->elementSize, sizeof(int));
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_clear(vector), CODE_MEMORY_ERROR);
}

static void test_vector_free(void **state)
{
    // Vector of integers
    Vector *integerVector = createIntegerVector();
    assert_int_equal(vector_free(integerVector), CODE_OK);
    free(integerVector);

    // Vector of C strings
    Vector *stringVector = createStringVector();
    assert_int_equal(vector_free(stringVector), CODE_OK);
    free(stringVector);
}

static void test_vector_get(void **state)
{
    Vector *vector = createIntegerVector();
    int *extracted = vector_at(vector, 5);
    assert_ptr_not_equal(extracted, NULL);
    assert_int_equal(*extracted, 5);
    assert_ptr_equal(vector_at(vector, 10), NULL);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_at(vector, 0), NULL);
}

static void test_vector_set(void **state)
{
    Vector *vector = createIntegerVector();
    int modification = 257;
    assert_int_equal(vector_set(vector, 5, &modification), CODE_OK);
    int *inside = vector->data + 5 * sizeof(int);
    assert_int_equal(*inside, 257);
    assert_int_equal(vector_set(vector, 10, &modification), CODE_LOGIC_ERROR);
    assert_int_equal(vector_set(vector, 5, NULL), CODE_MEMORY_ERROR);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_set(vector, 0, &modification), CODE_MEMORY_ERROR);
}

static void test_vector_begin(void **state)
{
    Iterator begin;

    Vector *vector = createIntegerVector();
    begin = vector_begin(vector);
    assert_ptr_equal(begin.pointer, vector->data);
    assert_int_equal(begin.size, vector->elementSize);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = vector_create(sizeof(int), noFree);
    begin = vector_begin(vector);
    assert_ptr_equal(begin.pointer, NULL);
    assert_int_equal(begin.size, 0);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    begin = vector_begin(vector);
    assert_ptr_equal(begin.pointer, NULL);
    assert_int_equal(begin.size, 0);
}

static void test_vector_end(void **state)
{
    Iterator end;

    Vector *vector = createIntegerVector();
    end = vector_end(vector);
    assert_ptr_equal(end.pointer, vector->data + 10 * sizeof(int));
    assert_int_equal(end.size, vector->elementSize);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = vector_create(sizeof(int), noFree);
    end = vector_end(vector);
    assert_ptr_equal(end.pointer, NULL);
    assert_int_equal(end.size, 0);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    end = vector_end(vector);
    assert_ptr_equal(end.pointer, NULL);
    assert_int_equal(end.size, 0);
}

static void test_vector_erase(void **state)
{
    Vector *vector = NULL;
    Iterator first, last;

    // Vector of integers
    vector = createIntegerVector();
    // Remove the element at index 1
    first = iterator_increase(vector_begin(vector), 1);
    last = iterator_increase(first, 1);
    assert_int_equal(vector_erase(vector, first, last), CODE_OK);
    assert_int_equal(vector->size, 9);
    assert_int_equal(vector->capacity, 10);
    int expectedInt = 0;
    for (size_t i = 0; i < 9; i++)
    {
        if (i == 1)
        {
            expectedInt++;
        }
        assert_int_equal(*(int *)(vector->data + i * vector->elementSize), expectedInt);
        expectedInt++;
    }
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    // Vector of strings
    vector = createStringVector();
    // Remove the element at index 1
    first = iterator_increase(vector_begin(vector), 1);
    last = iterator_increase(first, 1);
    assert_int_equal(vector_erase(vector, first, last), CODE_OK);
    assert_int_equal(vector->size, 9);
    assert_int_equal(vector->capacity, 10);
    char expectedChar = '0';
    for (size_t i = 0; i < 9; i++)
    {
        if (i == 1)
        {
            expectedChar++;
        }
        assert_int_equal(**(char **)(vector->data + i * vector->elementSize), expectedChar);
        expectedChar++;
    }
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);
}