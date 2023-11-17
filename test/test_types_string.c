#include <string.h>

#include "types/types_string.h"

static void test_types_string_create(void **state)
{
    String *string = types_string_create();
    assert_ptr_not_equal(string, NULL);
    assert_ptr_not_equal(string->buffer, NULL);
    assert_string_equal(string->buffer, "");
    assert_int_equal(strlen(string->buffer), 0);
    assert_int_equal(string->length, 0);
    assert_int_equal(string->capacity, 1);
    types_string_free(string);
    free(string);
}

static void test_types_string_create_from_literal(void **state)
{
    // Test correct value
    const char *literal = "My literal string";
    const size_t expected_length = strlen(literal);
    String *string = types_string_create_from_literal(literal);
    assert_ptr_not_equal(string, NULL);
    assert_ptr_not_equal(string->buffer, NULL);
    assert_string_equal(string->buffer, literal);
    assert_int_equal(strlen(string->buffer), expected_length);
    assert_int_equal(string->length, expected_length);
    assert_int_equal(string->capacity, expected_length + 1);
    types_string_free(string);
    free(string);

    // Test invalid value
    string = types_string_create_from_literal(NULL);
    assert_ptr_equal(string, NULL);
}

static void test_types_string_create_from_buffer(void **state)
{
    String *string2;

    // Test correct value
    const char *string1 = "My literal string";
    string2 = types_string_create_from_buffer(string1, strlen(string1));
    assert_ptr_not_equal(string2, NULL);
    assert_ptr_not_equal(string2->buffer, NULL);
    assert_string_equal(string2->buffer, string1);
    assert_int_equal(string2->length, strlen(string1));
    assert_int_equal(string2->capacity, strlen(string1) + 1);
    types_string_free(string2);
    free(string2);

    // Test with buffer of length zero
    string1 = "";
    string2 = types_string_create_from_buffer(string1, 0);
    assert_ptr_not_equal(string2, NULL);
    assert_ptr_not_equal(string2->buffer, NULL);
    assert_string_equal(string2->buffer, "");
    assert_int_equal(string2->length, 0);
    assert_int_equal(string2->capacity, 1);
    types_string_free(string2);
    free(string2);

    // Test invalid values
    string2 = types_string_create_from_buffer(NULL, 12);
    assert_ptr_equal(string2, NULL);
}

static void test_types_string_length(void **state)
{
    // Test valid values
    const char *literal = "My literal string";
    String *string = types_string_create_from_literal(literal);
    assert_int_equal(types_string_length(string), strlen(literal));
    types_string_free(string);
    free(string);

    // Test empty string
    string = types_string_create_from_literal("");
    assert_int_equal(types_string_length(string), 0);
    types_string_free(string);
    free(string);

    // Test invalid values
    assert_ptr_equal(types_string_length(NULL), 0);
}

static void test_types_string_c_str(void **state)
{
    // Test valid values
    String *string = types_string_create_from_literal("My literal string");
    assert_ptr_equal(types_string_c_str(string), string->buffer);
    types_string_free(string);
    free(string);

    // Test empty string
    string = types_string_create_from_literal("");
    assert_ptr_equal(types_string_c_str(string), string->buffer);
    types_string_free(string);
    free(string);

    // Test invalid parameter
    assert_ptr_equal(types_string_c_str(NULL), NULL);
}

static void test_types_string_at(void **state)
{
    // Test valid value
    const char *literal = "My literal string";
    const size_t index = 6;
    String *string = types_string_create_from_literal(literal);
    assert_int_equal((int)types_string_at(string, index), (int)literal[index]);
    types_string_free(string);
    free(string);
}

static void test_types_string_copy(void **state)
{
    String *string1, *string2;

    // Test strings with OK values
    string1 = types_string_create_from_literal("My literal string");
    string2 = types_string_copy(string1);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Copy from a NULL string
    assert_ptr_equal(types_string_copy(NULL), NULL);
}

static void test_types_string_compare(void **state)
{
    String *string1, *string2;
    int result;
    // Check strings with good values, different
    string1 = types_string_create_from_literal("My literal string");
    string2 = types_string_create();
    result = types_string_compare(string1, string2);
    assert_int_not_equal(result, 0);
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Check strings with good values, equal
    string1 = types_string_create_from_literal("My literal string");
    string2 = types_string_create_from_literal("My literal string");
    result = types_string_compare(string1, string2);
    assert_int_equal(result, 0);
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);
}

static void test_types_string_join_in_place(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test two random strings
    string1 = types_string_create_from_literal("string1");
    string2 = types_string_create_from_literal("string2");
    result = types_string_join_in_place(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1string2");
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Test with empty first string
    string1 = types_string_create();
    string2 = types_string_create_from_literal("string2");
    result = types_string_join_in_place(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string2");
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Test with empty second string
    string1 = types_string_create_from_literal("string1");
    string2 = types_string_create();
    result = types_string_join_in_place(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1");
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Two empty strings
    string1 = types_string_create();
    string2 = types_string_create();
    result = types_string_join_in_place(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "");
    types_string_free(string1);
    types_string_free(string2);
    free(string1);
    free(string2);

    // Test some invalid values
    string1 = types_string_create();
    result = types_string_join_in_place(string1, NULL);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    types_string_free(string1);
    free(string1);

    string2 = types_string_create();
    result = types_string_join_in_place(NULL, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    types_string_free(string2);
    free(string2);
}

static void test_types_string_join(void **state)
{
    String *string1, *string2, *result;

    // Test two random strings
    string1 = types_string_create_from_literal("string1");
    string2 = types_string_create_from_literal("string2");
    result = types_string_join(string1, string2);
    assert_string_equal(result->buffer, "string1string2");
    types_string_free(string1);
    types_string_free(string2);
    types_string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty first string
    string1 = types_string_create();
    string2 = types_string_create_from_literal("string2");
    result = types_string_join(string1, string2);
    assert_string_equal(result->buffer, "string2");
    types_string_free(string1);
    types_string_free(string2);
    types_string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty second string
    string1 = types_string_create_from_literal("string1");
    string2 = types_string_create();
    result = types_string_join(string1, string2);
    assert_string_equal(result->buffer, "string1");
    types_string_free(string1);
    types_string_free(string2);
    types_string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Two empty strings
    string1 = types_string_create();
    string2 = types_string_create();
    result = types_string_join(string1, string2);
    assert_string_equal(result->buffer, "");
    types_string_free(string1);
    types_string_free(string2);
    types_string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test some invalid values
    string1 = types_string_create();
    assert_int_equal(types_string_join(string1, NULL), NULL);
    types_string_free(string1);
    free(string1);

    string2 = types_string_create();
    assert_int_equal(types_string_join(NULL, string2), NULL);
    types_string_free(string2);
    free(string2);
}

static void test_types_string_reserve(void **state)
{
    // Test reserve from empty string
    String *string = types_string_create();
    assert_int_equal(types_string_reserve(string, 1000), CODE_OK);
    assert_int_equal(string->capacity, 1000);
    assert_int_equal(string->length, 0);
    assert_string_equal(string->buffer, "");
    types_string_free(string);
    free(string);

    // Test reserve from populated string
    string = types_string_create_from_literal("hello");
    assert_int_equal(types_string_reserve(string, 1000), CODE_OK);
    assert_int_equal(string->capacity, 1000);
    assert_int_equal(string->length, 5);
    assert_string_equal(string->buffer, "hello");
    types_string_free(string);
    free(string);

    // Test no need to reserve
    string = types_string_create_from_literal("hello");
    assert_int_equal(types_string_reserve(string, 2), CODE_OK);
    assert_int_equal(string->capacity, 6);
    assert_int_equal(string->length, 5);
    assert_string_equal(string->buffer, "hello");
    types_string_free(string);
    free(string);

    // Test invalid parameter
    assert_int_equal(types_string_reserve(NULL, 1000), CODE_MEMORY_ERROR);
}

static void test_types_string_free(void **state)
{
    // Test valid string
    String *string = types_string_create_from_literal("hello");
    assert_int_equal(types_string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test empty string
    string = types_string_create_from_literal("");
    assert_int_equal(types_string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test internal NULL string
    string = malloc(sizeof(String));
    string->buffer = NULL;
    string->capacity = 0;
    string->length = 0;
    assert_int_equal(types_string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test NULL parameter
    assert_int_equal(types_string_free(NULL), CODE_OK);
}
