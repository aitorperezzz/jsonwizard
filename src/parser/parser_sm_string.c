#include "parser_sm_string.h"
#include "parser_sm.h"
#include "parser_callbacks.h"

enum ParserSmStringState
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
    SM_STRING_NONE
};

static TransitionDef parser_sm_string_defs[43] =
    {
        {SM_STRING_FIRST, quote, SM_STRING_QUOTATION_MARK_START},
        {SM_STRING_QUOTATION_MARK_START, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_QUOTATION_MARK_START, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_QUOTATION_MARK_START, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_CODEPOINT, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_CODEPOINT, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_CODEPOINT, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_REVERSE_SOLIDUS_START, quote, SM_STRING_QUOTATION_MARK},
        {SM_STRING_REVERSE_SOLIDUS_START, reverse_solidus, SM_STRING_REVERSE_SOLIDUS},
        {SM_STRING_REVERSE_SOLIDUS_START, solidus, SM_STRING_SOLIDUS},
        {SM_STRING_REVERSE_SOLIDUS_START, backspace, SM_STRING_BACKSPACE},
        {SM_STRING_REVERSE_SOLIDUS_START, form_feed, SM_STRING_FORMFEED},
        {SM_STRING_REVERSE_SOLIDUS_START, line_feed, SM_STRING_LINEFEED},
        {SM_STRING_REVERSE_SOLIDUS_START, carriage_return, SM_STRING_CARRIAGE_RETURN},
        {SM_STRING_REVERSE_SOLIDUS_START, horizontal_tab, SM_STRING_HORIZONTAL_TAB},
        {SM_STRING_REVERSE_SOLIDUS_START, unicode, SM_STRING_UNICODE},
        {SM_STRING_QUOTATION_MARK, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_QUOTATION_MARK, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_QUOTATION_MARK, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_REVERSE_SOLIDUS, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_REVERSE_SOLIDUS, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_REVERSE_SOLIDUS, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_SOLIDUS, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_SOLIDUS, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_SOLIDUS, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_BACKSPACE, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_BACKSPACE, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_BACKSPACE, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_FORMFEED, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_FORMFEED, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_FORMFEED, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_LINEFEED, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_LINEFEED, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_LINEFEED, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_CARRIAGE_RETURN, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_CARRIAGE_RETURN, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_CARRIAGE_RETURN, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_HORIZONTAL_TAB, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_HORIZONTAL_TAB, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_HORIZONTAL_TAB, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
        {SM_STRING_UNICODE, quote, SM_STRING_QUOTATION_MARK_END},
        {SM_STRING_UNICODE, code_point, SM_STRING_CODEPOINT},
        {SM_STRING_UNICODE, reverse_solidus, SM_STRING_REVERSE_SOLIDUS_START},
};

StateMachine *parser_sm_string_define()
{
    StateMachine *sm = parser_sm_create();

    // Create all states following the enum
    for (int i = SM_STRING_FIRST; i < SM_STRING_NONE; i++)
    {
        parser_sm_add_state(sm, i);
    }

    // Define the transitions for each state
    parser_sm_add_tranistions(sm, parser_sm_string_defs, 43);

    // Define the acceptance state
    parser_sm_define_acceptance_state(sm, SM_STRING_QUOTATION_MARK_END);

    return sm;
}