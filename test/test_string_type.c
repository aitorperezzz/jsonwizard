#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "string_type.c"

static void test_from_literal(void **state)
{
    const char *literal = "My literal string";
    const size_t expectedLength = strlen(literal);
    String *string = stringCreateFromLiteral(literal);
    assert_string_equal(string->buffer, literal);
    assert_int_equal(strlen(string->buffer), expectedLength);
    assert_int_equal(string->length, expectedLength);
    assert_int_equal(string->capacity, expectedLength + 1);
}

static void test_join_in_place(void **state)
{
    String *string1, *string2;
    ResultCode result;

    // Test two random strings
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreateFromLiteral("string2");
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, JSON_OK);
    assert_string_equal(string1->buffer, "string1string2");
    stringFree(string1);
    stringFree(string2);

    // Test with empty first string
    string1 = stringCreate();
    string2 = stringCreateFromLiteral("string2");
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, JSON_OK);
    assert_string_equal(string1->buffer, "string2");
    stringFree(string1);
    stringFree(string2);

    // Test with empty second string
    string1 = stringCreateFromLiteral("string1");
    string2 = stringCreate();
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, JSON_OK);
    assert_string_equal(string1->buffer, "string1");
    stringFree(string1);
    stringFree(string2);

    // Two empty strings
    string1 = stringCreate();
    string2 = stringCreate();
    result = stringJoinInPlace(string1, string2);
    assert_int_equal(result, JSON_OK);
    assert_string_equal(string1->buffer, "");
    stringFree(string1);
    stringFree(string2);
}

static void test_copy(void **state)
{
    String *string1 = stringCreateFromLiteral("My literal string");
    String *string2 = stringCreate();
    ResultCode result = stringCopy(string2, string1);
    assert_int_equal(result, JSON_OK);
    assert_string_equal(string2->buffer, string1->buffer);
}

static void test_copy_null(void **state)
{
    String *string1 = NULL;
    String *string2 = stringCreate();
    ResultCode result = stringCopy(string2, string1);
    assert_int_equal(result, JSON_MEMORY_ERROR);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_from_literal),
        cmocka_unit_test(test_join_in_place),
        cmocka_unit_test(test_copy),
        cmocka_unit_test(test_copy_null),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}