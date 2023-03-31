#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "string_type.c"

static void testCreate(void **state)
{
    String *string = stringCreate();
    const size_t expectedLength = 0;
    assert_string_equal(string->buffer, "");
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
    stringReset(string);
    free(string);
}

static void testCreateFromLiteral(void **state)
{
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = stringCreateFromLiteral(literal);
    assert_string_equal(string->buffer, literal);
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
    stringReset(string);
    free(string);
}

static void testGetLength(void **state)
{
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = stringCreateFromLiteral(literal);
    assert_int_equal(stringGetLength(string), expectedLength);
    stringReset(string);
    free(string);
}

static void testGetBuffer(void **state)
{
    const char *literal = "My literal string";
    String *string = stringCreateFromLiteral(literal);
    assert_int_equal(stringGetBuffer(string), string->buffer);
    stringReset(string);
    free(string);
}

static void testGetChar(void **state)
{
    const char *literal = "My literal string";
    const size_t index = 6;
    const char expectedChar = literal[index];
    String *string = stringCreateFromLiteral(literal);
    assert_int_equal((int)stringGetChar(string, index), (int)expectedChar);
    stringReset(string);
    free(string);
}

static void testCopy(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test strings with OK values
    string1 = stringCreateFromLiteral("My literal string");
    string2 = stringCreate();
    result = stringCopy(string2, string1);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Copy to a NULL string
    string1 = NULL;
    string2 = stringCreate();
    result = stringCopy(string1, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Copy from a NULL string
    string1 = stringCreate();
    string2 = NULL;
    result = stringCopy(string1, string2);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);
}

static void testCopyFromBuffer(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test strings with OK values
    string1 = stringCreateFromLiteral("My literal string");
    string2 = stringCreate();
    result = stringCopyFromBuffer(string2, string1->buffer, string1->length);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string2->buffer, string1->buffer);
    assert_int_equal(string2->length, string1->length);
    assert_int_equal(string2->capacity, string1->capacity);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Copy to a NULL string
    string1 = NULL;
    string2 = stringCreate();
    result = stringCopyFromBuffer(string1, string2->buffer, string2->length);
    assert_int_equal(result, CODE_MEMORY_ERROR);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);
}

static void testCompare(void **state)
{
    String *string1, *string2;
    int result;
    // Check strings with good values, different
    string1 = stringCreateFromLiteral("My literal string");
    string2 = stringCreate();
    result = stringCompare(string1, string2);
    assert_int_not_equal(result, 0);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Check strings with good values, equal
    string1 = stringCreateFromLiteral("My literal string");
    string2 = stringCreateFromLiteral("My literal string");
    result = stringCompare(string1, string2);
    assert_int_equal(result, 0);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Check strings with bad values
    string1 = NULL;
    string2 = stringCreate();
    result = stringCompare(string1, string2);
    assert_int_not_equal(result, 0);
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);
}

static void testJoinInPlace(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test two random strings
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreateFromLiteral("string2");
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1string2");
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Test with empty first string
    string1 = stringCreate();
    string2 = stringCreateFromLiteral("string2");
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string2");
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Test with empty second string
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreate();
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "string1");
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);

    // Two empty strings
    string1 = stringCreate();
    string2 = stringCreate();
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, CODE_OK);
    assert_string_equal(string1->buffer, "");
    stringReset(string1);
    stringReset(string2);
    free(string1);
    free(string2);
}

static void testJoin(void **state)
{
    String *string1, *string2, *result;

    // Test two random strings
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreateFromLiteral("string2");
    result = stringJoin(string1, string2);
    assert_string_equal(result->buffer, "string1string2");
    stringReset(string1);
    stringReset(string2);
    stringReset(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty first string
    string1 = stringCreate();
    string2 = stringCreateFromLiteral("string2");
    result = stringJoin(string1, string2);
    assert_string_equal(result->buffer, "string2");
    stringReset(string1);
    stringReset(string2);
    stringReset(result);
    free(string1);
    free(string2);
    free(result);

    // Test with empty second string
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreate();
    result = stringJoin(string1, string2);
    assert_string_equal(result->buffer, "string1");
    stringReset(string1);
    stringReset(string2);
    stringReset(result);
    free(string1);
    free(string2);
    free(result);

    // Two empty strings
    string1 = stringCreate();
    string2 = stringCreate();
    result = stringJoin(string1, string2);
    assert_string_equal(result->buffer, "");
    stringReset(string1);
    stringReset(string2);
    stringReset(result);
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