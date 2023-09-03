#include <string.h>

#include "string_type.h"

static void test_string_type_create(void **state)
{
    String *string = string_create();
    assert_ptr_not_equal(string, NULL);
    assert_ptr_not_equal(string->buffer, NULL);
    assert_string_equal(string->buffer, "");
    assert_int_equal(strlen(string->buffer), 0);
    assert_int_equal(string->length, 0);
    assert_int_equal(string->capacity, 1);
    string_free(string);
    free(string);
}

static void test_string_type_createFromLiteral(void **state)
{
    // Test correct value
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = string_createFromLiteral(literal);
    assert_ptr_not_equal(string, NULL);
    assert_ptr_not_equal(string->buffer, NULL);
    assert_string_equal(string->buffer, literal);
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
    string_free(string);
    free(string);

    // Test invalid value
    string = string_createFromLiteral(NULL);
    assert_ptr_equal(string, NULL);
}

static void test_string_type_createFromBuffer(void **state)
{
    String *string2;

    // Test correct value
    const char *string1 = "My literal string";
    string2 = string_createFromBuffer(string1, strlen(string1));
    assert_ptr_not_equal(string2, NULL);
    assert_ptr_not_equal(string2->buffer, NULL);
    assert_string_equal(string2->buffer, string1);
    assert_int_equal(string2->length, strlen(string1));
    assert_int_equal(string2->capacity, strlen(string1) + 1);
    string_free(string2);
    free(string2);

    // Test with buffer of length zero
    string1 = "";
    string2 = string_createFromBuffer(string1, 0);
    assert_ptr_not_equal(string2, NULL);
    assert_ptr_not_equal(string2->buffer, NULL);
    assert_string_equal(string2->buffer, "");
    assert_int_equal(string2->length, 0);
    assert_int_equal(string2->capacity, 1);
    string_free(string2);
    free(string2);

    // Test invalid values
    string2 = string_createFromBuffer(NULL, 12);
    assert_ptr_equal(string2, NULL);
}

static void test_string_type_length(void **state)
{
    // Test valid values
    const char *literal = "My literal string";
    String *string = string_createFromLiteral(literal);
    assert_int_equal(string_length(string), strlen(literal));
    string_free(string);
    free(string);

    // Test empty string
    string = string_createFromLiteral("");
    assert_int_equal(string_length(string), 0);
    string_free(string);
    free(string);

    // Test invalid values
    assert_ptr_equal(string_length(NULL), 0);
}

static void test_string_type_cStr(void **state)
{
    // Test valid values
    String *string = string_createFromLiteral("My literal string");
    assert_ptr_equal(string_cStr(string), string->buffer);
    string_free(string);
    free(string);

    // Test empty string
    string = string_createFromLiteral("");
    assert_ptr_equal(string_cStr(string), string->buffer);
    string_free(string);
    free(string);

    // Test invalid parameter
    assert_ptr_equal(string_cStr(NULL), NULL);
}

static void test_string_type_at(void **state)
{
    // Test valid value
    const char *literal = "My literal string";
    const size_t index = 6;
    String *string = string_createFromLiteral(literal);
    assert_int_equal((int)string_at(string, index), (int)literal[index]);
    string_free(string);
    free(string);
}

static void test_string_type_copy(void **state)
{
    String *string1, *string2;

    // Test strings with OK values
    string1 = string_createFromLiteral("My literal string");
    string2 = string_copy(string1);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Copy from a NULL string
    assert_ptr_equal(string_copy(NULL), NULL);
}

static void test_string_type_compare(void **state)
{
    String *string1, *string2;
    int result;
    // Check strings with good values, different
    string1 = string_createFromLiteral("My literal string");
    string2 = string_create();
    result = string_compare(string1, string2);
    assert_int_not_equal(result, 0);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Check strings with good values, equal
    string1 = string_createFromLiteral("My literal string");
    string2 = string_createFromLiteral("My literal string");
    result = string_compare(string1, string2);
    assert_int_equal(result, 0);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);
}

static void test_string_type_joinInPlace(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test two random strings
    string1 = string_createFromLiteral("string1");
    string2 = string_createFromLiteral("string2");
    result = string_joinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1string2");
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Test with empty first string
    string1 = string_create();
    string2 = string_createFromLiteral("string2");
    result = string_joinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string2");
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Test with empty second string
    string1 = string_createFromLiteral("string1");
    string2 = string_create();
    result = string_joinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1");
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Two empty strings
    string1 = string_create();
    string2 = string_create();
    result = string_joinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "");
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Test some invalid values
    string1 = string_create();
    result = string_joinInPlace(string1, NULL);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    string_free(string1);
    free(string1);

    string2 = string_create();
    result = string_joinInPlace(NULL, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    string_free(string2);
    free(string2);
}

static void test_string_type_join(void **state)
{
    String *string1, *string2, *result;

    // Test two random strings
    string1 = string_createFromLiteral("string1");
    string2 = string_createFromLiteral("string2");
    result = string_join(string1, string2);
    assert_string_equal(result->buffer, "string1string2");
    string_free(string1);
    string_free(string2);
    string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty first string
    string1 = string_create();
    string2 = string_createFromLiteral("string2");
    result = string_join(string1, string2);
    assert_string_equal(result->buffer, "string2");
    string_free(string1);
    string_free(string2);
    string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty second string
    string1 = string_createFromLiteral("string1");
    string2 = string_create();
    result = string_join(string1, string2);
    assert_string_equal(result->buffer, "string1");
    string_free(string1);
    string_free(string2);
    string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Two empty strings
    string1 = string_create();
    string2 = string_create();
    result = string_join(string1, string2);
    assert_string_equal(result->buffer, "");
    string_free(string1);
    string_free(string2);
    string_free(result);
    free(string1);
    free(string2);
    free(result);

    // Test some invalid values
    string1 = string_create();
    assert_int_equal(string_join(string1, NULL), NULL);
    string_free(string1);
    free(string1);

    string2 = string_create();
    assert_int_equal(string_join(NULL, string2), NULL);
    string_free(string2);
    free(string2);
}

static void test_string_type_reserve(void **state)
{
    // Test reserve from empty string
    String *string = string_create();
    assert_int_equal(string_reserve(string, 1000), CODE_OK);
    assert_int_equal(string->capacity, 1000);
    assert_int_equal(string->length, 0);
    assert_string_equal(string->buffer, "");
    string_free(string);
    free(string);

    // Test reserve from populated string
    string = string_createFromLiteral("hello");
    assert_int_equal(string_reserve(string, 1000), CODE_OK);
    assert_int_equal(string->capacity, 1000);
    assert_int_equal(string->length, 5);
    assert_string_equal(string->buffer, "hello");
    string_free(string);
    free(string);

    // Test no need to reserve
    string = string_createFromLiteral("hello");
    assert_int_equal(string_reserve(string, 2), CODE_OK);
    assert_int_equal(string->capacity, 6);
    assert_int_equal(string->length, 5);
    assert_string_equal(string->buffer, "hello");
    string_free(string);
    free(string);

    // Test invalid parameter
    assert_int_equal(string_reserve(NULL, 1000), CODE_MEMORY_ERROR);
}

static void test_string_type_free(void **state)
{
    // Test valid string
    String *string = string_createFromLiteral("hello");
    assert_int_equal(string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test empty string
    string = string_createFromLiteral("");
    assert_int_equal(string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test internal NULL string
    string = malloc(sizeof(String));
    string->buffer = NULL;
    string->capacity = 0;
    string->length = 0;
    assert_int_equal(string_free(string), CODE_OK);
    assert_ptr_equal(string->buffer, NULL);
    assert_ptr_equal(string->capacity, 0);
    assert_ptr_equal(string->length, 0);
    free(string);

    // Test NULL parameter
    assert_int_equal(string_free(NULL), CODE_OK);
}
