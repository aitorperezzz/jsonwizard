#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_types_string.c"
#include "test_types_iterator.c"
#include "test_types_vector.c"
#include "test_parser_sm_string.c"

int main(void)
{
    const struct CMUnitTest tests[] = {
        // string
        cmocka_unit_test(test_types_string_create),
        cmocka_unit_test(test_types_string_create_from_literal),
        cmocka_unit_test(test_types_string_create_from_buffer),
        cmocka_unit_test(test_types_string_length),
        cmocka_unit_test(test_types_string_c_str),
        cmocka_unit_test(test_types_string_at),
        cmocka_unit_test(test_types_string_copy),
        cmocka_unit_test(test_types_string_compare),
        cmocka_unit_test(test_types_string_join_in_place),
        cmocka_unit_test(test_types_string_join),
        cmocka_unit_test(test_types_string_reserve),
        cmocka_unit_test(test_types_string_free),
        // iterator
        cmocka_unit_test(test_types_iterator_create),
        cmocka_unit_test(test_types_iterator_invalid),
        cmocka_unit_test(test_types_iterator_copy),
        cmocka_unit_test(test_types_iterator_find),
        cmocka_unit_test(test_types_iterator_equal),
        cmocka_unit_test(test_types_iterator_get),
        cmocka_unit_test(test_types_iterator_distance),
        cmocka_unit_test(test_types_iterator_increase),
        cmocka_unit_test(test_types_iterator_decrease),
        // vector
        cmocka_unit_test(test_types_vector_create),
        cmocka_unit_test(test_types_vector_size),
        cmocka_unit_test(test_types_vector_push),
        cmocka_unit_test(test_types_vector_clear),
        cmocka_unit_test(test_types_vector_free),
        cmocka_unit_test(test_types_vector_at),
        cmocka_unit_test(test_types_vector_set),
        cmocka_unit_test(test_types_vector_begin),
        cmocka_unit_test(test_types_vector_end),
        cmocka_unit_test(test_types_vector_erase),
        cmocka_unit_test(test_types_vector_insert),
        cmocka_unit_test(test_types_vector_empty),
        // read
        cmocka_unit_test(test_read_sm),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}