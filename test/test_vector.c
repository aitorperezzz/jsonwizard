#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "vector.c"
#include "iterator.c"

static ResultCode noFree(void *data)
{
    return CODE_OK;
}

static Vector *createFilledVector(void)
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

static void test_create(void **state)
{
    Vector *vector = vector_create(sizeof(int), noFree);
    assert_ptr_equal(vector->data, NULL);
    assert_int_equal(vector->size, 0);
    assert_int_equal(vector->capacity, 0);
    assert_int_equal(vector->elementSize, sizeof(int));
    assert_ptr_equal(vector->freeCallback, noFree);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = vector_create(sizeof(int), NULL);
    assert_ptr_equal(vector, NULL);
}

static void test_size(void **state)
{
    Vector *vector = createFilledVector();
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

static void test_push(void **state)
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

static void test_clear(void **state)
{
    Vector *vector = createFilledVector();
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

static void test_freeVector(void **state)
{
}

static void test_get(void **state)
{
    Vector *vector = createFilledVector();
    int *extracted = vector_get(vector, 5);
    assert_ptr_not_equal(extracted, NULL);
    assert_int_equal(*extracted, 5);
    assert_ptr_equal(vector_get(vector, 10), NULL);
    assert_int_equal(vector_free(vector), CODE_OK);
    free(vector);

    vector = NULL;
    assert_int_equal(vector_get(vector, 0), NULL);
}

static void test_set(void **state)
{
    Vector *vector = createFilledVector();
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

static void test_begin(void **state)
{
    Iterator begin;

    Vector *vector = createFilledVector();
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

static void test_end(void **state)
{
    Iterator end;

    Vector *vector = createFilledVector();
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

static void test_erase(void **state)
{
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_size),
        cmocka_unit_test(test_push),
        cmocka_unit_test(test_clear),
        cmocka_unit_test(test_freeVector),
        cmocka_unit_test(test_get),
        cmocka_unit_test(test_set),
        cmocka_unit_test(test_begin),
        cmocka_unit_test(test_end),
        cmocka_unit_test(test_erase),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}