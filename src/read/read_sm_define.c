#include "read_sm_define.h"
#include "read_sm.h"
#include "read_sm_callbacks.h"

enum ReadSmStringState
{
    SM_STRING_FIRST,
    SM_STRING_QUOTATION_MARK_START,
    SM_STRING_QUOTATION_MARK_END,
    SM_STRING_CODEPOINT,
    SM_STRING_REVERSE_SOLIDUS_START,
    SM_STRING_QUOTATION_MARK,
    SM_STRING_REVERSE_SOLIDUS,
    SM_STRING_SOLIDUS,
    SM_STRING_BACKSPACE,
    SM_STRING_FORMFEED,
    SM_STRING_LINEFEED,
    SM_STRING_CARRIAGE_RETURN,
    SM_STRING_HORIZONTAL_TAB,
    SM_STRING_UNICODE,
    SM_STRING_HEX_CHARACTER_1,
    SM_STRING_HEX_CHARACTER_2,
    SM_STRING_HEX_CHARACTER_3,
    SM_STRING_HEX_CHARACTER_4,
    SM_STRING_TOTAL
};

enum ReadSmNumberState
{
    SM_NUMBER_FIRST,
    SM_NUMBER_ZERO,
    SM_NUMBER_MINUS_WHOLE,
    SM_NUMBER_DIGIT_1TO9,
    SM_NUMBER_DIGIT_WHOLE,
    SM_NUMBER_DIGIT_FRACTION,
    SM_NUMBER_DIGIT_EXPONENT,
    SM_NUMBER_DOT,
    SM_NUMBER_E,
    SM_NUMBER_EXPONENT_SIGN,
    SM_NUMBER_TOTAL
};

enum ReadSmTrueState
{
    SM_TRUE_FIRST,
    SM_TRUE_T,
    SM_TRUE_R,
    SM_TRUE_U,
    SM_TRUE_E,
    SM_TRUE_TOTAL
};

enum ReadSmFalseState
{
    SM_FALSE_FIRST,
    SM_FALSE_F,
    SM_FALSE_A,
    SM_FALSE_L,
    SM_FALSE_S,
    SM_FALSE_E,
    SM_FALSE_TOTAL
};

enum ReadSmNullState
{
    SM_NULL_FIRST,
    SM_NULL_N,
    SM_NULL_U,
    SM_NULL_L,
    SM_NULL_L_SECOND,
    SM_NULL_TOTAL
};

enum ReadSmWhitespaceState
{
    SM_WHITESPACE_FIRST,
    SM_WHITESPACE_VALID,
    SM_WHITESPACE_TOTAL
};

#define READ_SM_DEFS_STRING_NUMBER 43
static TransitionDef read_sm_defs_string[READ_SM_DEFS_STRING_NUMBER] =
    {
        {SM_STRING_FIRST, callback_quote, SM_STRING_QUOTATION_MARK_START},

        {SM_STRING_QUOTATION_MARK_START, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_QUOTATION_MARK_START, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_QUOTATION_MARK_START, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_CODEPOINT, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_CODEPOINT, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_CODEPOINT, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_REVERSE_SOLIDUS_START, callback_quote, SM_STRING_QUOTATION_MARK},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_solidus, SM_STRING_SOLIDUS},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_backspace, SM_STRING_BACKSPACE},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_form_feed, SM_STRING_FORMFEED},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_line_feed, SM_STRING_LINEFEED},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_carriage_return, SM_STRING_CARRIAGE_RETURN},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_horizontal_tab, SM_STRING_HORIZONTAL_TAB},
        {SM_STRING_REVERSE_SOLIDUS_START, callback_unicode, SM_STRING_UNICODE},

        {SM_STRING_QUOTATION_MARK, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_QUOTATION_MARK, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_QUOTATION_MARK, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_REVERSE_SOLIDUS, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_REVERSE_SOLIDUS, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_REVERSE_SOLIDUS, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_SOLIDUS, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_SOLIDUS, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_SOLIDUS, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_BACKSPACE, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_BACKSPACE, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_BACKSPACE, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_FORMFEED, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_FORMFEED, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_FORMFEED, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_LINEFEED, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_LINEFEED, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_LINEFEED, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_CARRIAGE_RETURN, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_CARRIAGE_RETURN, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_CARRIAGE_RETURN, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_HORIZONTAL_TAB, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_HORIZONTAL_TAB, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_HORIZONTAL_TAB, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},

        {SM_STRING_UNICODE, callback_quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_UNICODE, callback_code_point, SM_STRING_CODEPOINT},
        {SM_STRING_UNICODE, callback_reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
};

#define READ_SM_DEFS_NUMBER_NUMBER 20
static TransitionDef read_sm_defs_number[READ_SM_DEFS_NUMBER_NUMBER] =
    {
        {SM_NUMBER_FIRST, callback_zero, SM_NUMBER_ZERO},
        {SM_NUMBER_FIRST, callback_minus, SM_NUMBER_MINUS_WHOLE},
        {SM_NUMBER_FIRST, callback_digit_1to9, SM_NUMBER_DIGIT_1TO9},

        {SM_NUMBER_ZERO, callback_dot, SM_NUMBER_DOT},
        {SM_NUMBER_ZERO, callback_e, SM_NUMBER_E},

        {SM_NUMBER_MINUS_WHOLE, callback_zero, SM_NUMBER_ZERO},
        {SM_NUMBER_MINUS_WHOLE, callback_digit_1to9, SM_NUMBER_DIGIT_1TO9},

        {SM_NUMBER_DIGIT_1TO9, callback_digit, SM_NUMBER_DIGIT_WHOLE},
        {SM_NUMBER_DIGIT_1TO9, callback_dot, SM_NUMBER_DOT},
        {SM_NUMBER_DIGIT_1TO9, callback_e, SM_NUMBER_E},

        {SM_NUMBER_DIGIT_WHOLE, callback_digit, SM_NUMBER_DIGIT_WHOLE},
        {SM_NUMBER_DIGIT_WHOLE, callback_dot, SM_NUMBER_DOT},

        {SM_NUMBER_DOT, callback_digit, SM_NUMBER_DIGIT_FRACTION},

        {SM_NUMBER_E, callback_minus, SM_NUMBER_EXPONENT_SIGN},
        {SM_NUMBER_E, callback_plus, SM_NUMBER_EXPONENT_SIGN},
        {SM_NUMBER_E, callback_digit, SM_NUMBER_DIGIT_EXPONENT},

        {SM_NUMBER_EXPONENT_SIGN, callback_digit, SM_NUMBER_DIGIT_EXPONENT},

        {SM_NUMBER_DIGIT_EXPONENT, callback_digit, SM_NUMBER_DIGIT_EXPONENT},

        {SM_NUMBER_DIGIT_FRACTION, callback_digit, SM_NUMBER_DIGIT_FRACTION},
        {SM_NUMBER_DIGIT_FRACTION, callback_exponent_e, SM_NUMBER_E},
};

#define READ_SM_DEFS_TRUE_NUMBER 4
static TransitionDef read_sm_defs_true[READ_SM_DEFS_TRUE_NUMBER] =
    {
        {SM_TRUE_FIRST, callback_t, SM_TRUE_T},
        {SM_TRUE_T, callback_r, SM_TRUE_R},
        {SM_TRUE_R, callback_u, SM_TRUE_U},
        {SM_TRUE_U, callback_e, SM_TRUE_E},
};

#define READ_SM_DEFS_FALSE_NUMBER 5
static TransitionDef read_sm_defs_false[READ_SM_DEFS_FALSE_NUMBER] =
    {
        {SM_FALSE_FIRST, callback_f, SM_FALSE_F},
        {SM_FALSE_F, callback_a, SM_FALSE_A},
        {SM_FALSE_A, callback_l, SM_FALSE_L},
        {SM_FALSE_L, callback_s, SM_FALSE_S},
        {SM_FALSE_S, callback_e, SM_FALSE_E},
};

#define READ_SM_DEFS_NULL_NUMBER 4
static TransitionDef read_sm_defs_null[READ_SM_DEFS_NULL_NUMBER] =
    {
        {SM_NULL_FIRST, callback_n, SM_NULL_N},
        {SM_NULL_N, callback_u, SM_NULL_U},
        {SM_NULL_U, callback_l, SM_NULL_L},
        {SM_NULL_L, callback_l, SM_NULL_L_SECOND},
};

#define READ_SM_DEFS_WHITESPACE_NUMBER 2
static TransitionDef read_sm_defs_whitespace[READ_SM_DEFS_WHITESPACE_NUMBER] =
    {
        {SM_WHITESPACE_FIRST, callback_whitespace, SM_WHITESPACE_VALID},
        {SM_WHITESPACE_VALID, callback_whitespace, SM_WHITESPACE_VALID},
};

StateMachine *read_sm_define_string()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_STRING_FIRST; i < SM_STRING_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_string, READ_SM_DEFS_STRING_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_STRING_QUOTATION_MARK_END);
    return sm;
}

StateMachine *read_sm_define_number()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_NUMBER_FIRST; i < SM_NUMBER_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_number, READ_SM_DEFS_NUMBER_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_NUMBER_ZERO);
    read_sm_define_acceptance_state(sm, SM_NUMBER_DIGIT_1TO9);
    read_sm_define_acceptance_state(sm, SM_NUMBER_DIGIT_WHOLE);
    read_sm_define_acceptance_state(sm, SM_NUMBER_DIGIT_FRACTION);
    read_sm_define_acceptance_state(sm, SM_NUMBER_DIGIT_EXPONENT);
    return sm;
}

StateMachine *read_sm_define_true()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_TRUE_FIRST; i < SM_TRUE_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_true, READ_SM_DEFS_TRUE_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_TRUE_E);
    return sm;
}

StateMachine *read_sm_define_false()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_FALSE_FIRST; i < SM_FALSE_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_false, READ_SM_DEFS_FALSE_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_FALSE_E);
    return sm;
}

StateMachine *read_sm_define_null()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_NULL_FIRST; i < SM_NULL_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_null, READ_SM_DEFS_NULL_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_NULL_L_SECOND);
    return sm;
}

StateMachine *read_sm_define_whitespace()
{
    StateMachine *sm = read_sm_create();
    // States
    for (int i = SM_WHITESPACE_FIRST; i < SM_WHITESPACE_TOTAL; i++)
    {
        read_sm_add_state(sm, i);
    }
    // Transitions
    read_sm_add_tranistions(sm, read_sm_defs_whitespace, READ_SM_DEFS_WHITESPACE_NUMBER);
    // Acceptance states
    read_sm_define_acceptance_state(sm, SM_WHITESPACE_VALID);
    return sm;
}