#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "string_type.c"

static void testCreate(void **state)
{
    String *string = string_create();
    const size_t expectedLength = 0;
    assert_string_equal(string->buffer, "");
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
    string_free(string);
    free(string);
}

static void testCreateFromLiteral(void **state)
{
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = string_createFromLiteral(literal);
    assert_string_equal(string->buffer, literal);
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
    string_free(string);
    free(string);
}

static void testGetLength(void **state)
{
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = string_createFromLiteral(literal);
    assert_int_equal(string_length(string), expectedLength);
    string_free(string);
    free(string);
}

static void testGetBuffer(void **state)
{
    const char *literal = "My literal string";
    String *string = string_createFromLiteral(literal);
    assert_int_equal(string_cStr(string), string->buffer);
    string_free(string);
    free(string);
}

static void testGetChar(void **state)
{
    const char *literal = "My literal string";
    const size_t index = 6;
    const char expectedChar = literal[index];
    String *string = string_createFromLiteral(literal);
    assert_int_equal((int)string_at(string, index), (int)expectedChar);
    string_free(string);
    free(string);
}

static void testCopy(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test strings with OK values
    string1 = string_createFromLiteral("My literal string");
    string2 = string_create();
    result = string_copy(string2, string1);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Copy to a NULL string
    string1 = NULL;
    string2 = string_create();
    result = string_copy(string1, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Copy from a NULL string
    string1 = string_create();
    string2 = NULL;
    result = string_copy(string1, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);
}

static void testCopyFromBuffer(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test strings with OK values
    string1 = string_createFromLiteral("My literal string");
    string2 = string_create();
    result = string_copyFromBuffer(string2, string1->buffer, string1->length);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);

    // Copy to a NULL string
    string1 = NULL;
    string2 = string_create();
    result = string_copyFromBuffer(string1, string2->buffer, string2->length);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);
}

static void testCompare(void **state)
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

    // Check strings with bad values
    string1 = NULL;
    string2 = string_create();
    result = string_compare(string1, string2);
    assert_int_not_equal(result, 0);
    string_free(string1);
    string_free(string2);
    free(string1);
    free(string2);
}

static void testJoinInPlace(void **state)
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
}

static void testJoin(void **state)
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
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testCreate),
        cmocka_unit_test(testCreateFromLiteral),
        cmocka_unit_test(testGetLength),
        cmocka_unit_test(testGetBuffer),
        cmocka_unit_test(testGetChar),
        cmocka_unit_test(testCopy),
        cmocka_unit_test(testCopyFromBuffer),
        cmocka_unit_test(testCompare),
        cmocka_unit_test(testJoinInPlace),
        cmocka_unit_test(testJoin),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}