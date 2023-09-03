#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_string_type.c"
#include "test_iterator.c"
#include "test_vector.c"

int main(void)
{
    const struct CMUnitTest tests[] = {
        // string_type
        cmocka_unit_test(test_string_type_create),
        cmocka_unit_test(test_string_type_createFromLiteral),
        cmocka_unit_test(test_string_type_getLength),
        cmocka_unit_test(test_string_type_getBuffer),
        cmocka_unit_test(test_string_type_getChar),
        cmocka_unit_test(test_string_type_copy),
        cmocka_unit_test(test_string_type_copyFromBuffer),
        cmocka_unit_test(test_string_type_compare),
        cmocka_unit_test(test_string_type_joinInPlace),
        cmocka_unit_test(test_string_type_join),
        // iterator
        cmocka_unit_test(test_iterator_create),
        cmocka_unit_test(test_iterator_copy),
        cmocka_unit_test(test_iterator_find),
        cmocka_unit_test(test_iterator_equal),
        cmocka_unit_test(test_iterator_get),
        cmocka_unit_test(test_iterator_distance),
        cmocka_unit_test(test_iterator_increase),
        cmocka_unit_test(test_iterator_decrease),
        // vector
        cmocka_unit_test(test_vector_create),
        cmocka_unit_test(test_vector_size),
        cmocka_unit_test(test_vector_push),
        cmocka_unit_test(test_vector_clear),
        cmocka_unit_test(test_vector_free),
        cmocka_unit_test(test_vector_get),
        cmocka_unit_test(test_vector_set),
        cmocka_unit_test(test_vector_begin),
        cmocka_unit_test(test_vector_end),
        cmocka_unit_test(test_vector_erase),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}