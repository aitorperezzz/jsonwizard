

#include "parser_sm.h"

bool quote(const char c)
{
    return &c == "\"";
}

bool codePoint(const char c)
{
    return c != (char)"\"" && c != (char)"\\" && c >= 31;
}

bool reverseSolidus(const char c)
{
    return c == (char)"\\";
}

bool solidus(const char c)
{
    return c == (char)"\\";
}

bool backspace(const char c)
{
    return c == (char)"\\";
}

bool formfeed(const char c)
{
    return c == (char)"\\";
}

bool linefeed(const char c)
{
    return c == (char)"\\";
}

bool carriageReturn(const char c)
{
    return c == (char)"\\";
}

bool horizontalTab(const char c)
{
    return c == (char)"\\";
}

bool unicode(const char c)
{
    return c == (char)"\\";
}

enum ParserSmStringState
{
    QUOTATION_MARK_START,
    QUOTATION_MARK_END,
    CODEPOINT,
    REVERSE_SOLIDUS_START,
    QUOTATION_MARK,
    REVERSE_SOLIDUS,
    SOLIDUS,
    BACKSPACE,
    FORMFEED,
    LINEFEED,
    CARRIAGE_RETURN,
    HORIZONTAL_TAB,
    UNICODE,
    HEX_CHARACTER_1,
    HEX_CHARACTER_2,
    HEX_CHARACTER_3,
    HEX_CHARACTER_4,
};

TransitionDef parserSmStringDef[40] =
    {
        {QUOTATION_MARK_START, quote, QUOTATION_MARK_END},
        {QUOTATION_MARK_START, codePoint, CODEPOINT},
        {QUOTATION_MARK_START, reverseSolidus, REVERSE_SOLIDUS_START},
        {CODEPOINT, quote, QUOTATION_MARK_END},
        {CODEPOINT, codePoint, CODEPOINT},
        {CODEPOINT, reverseSolidus, REVERSE_SOLIDUS_START},
        {REVERSE_SOLIDUS_START, quote, QUOTATION_MARK},
        {REVERSE_SOLIDUS_START, reverseSolidus, REVERSE_SOLIDUS},
        {REVERSE_SOLIDUS_START, solidus, SOLIDUS},
        {REVERSE_SOLIDUS_START, backspace, BACKSPACE},
        {REVERSE_SOLIDUS_START, formfeed, FORMFEED},
        {REVERSE_SOLIDUS_START, linefeed, LINEFEED},
        {REVERSE_SOLIDUS_START, carriageReturn, CARRIAGE_RETURN},
        {REVERSE_SOLIDUS_START, horizontalTab, HORIZONTAL_TAB},
        {REVERSE_SOLIDUS_START, unicode, UNICODE},
        {QUOTATION_MARK, quote, QUOTATION_MARK_END},
        {QUOTATION_MARK, codePoint, CODEPOINT},
        {QUOTATION_MARK, reverseSolidus, REVERSE_SOLIDUS_START},
        {REVERSE_SOLIDUS, quote, QUOTATION_MARK_END},
        {REVERSE_SOLIDUS, codePoint, CODEPOINT},
        {REVERSE_SOLIDUS, reverseSolidus, REVERSE_SOLIDUS_START},
        {SOLIDUS, quote, QUOTATION_MARK_END},
        {SOLIDUS, codePoint, CODEPOINT},
        {SOLIDUS, reverseSolidus, REVERSE_SOLIDUS_START},
        {BACKSPACE, quote, QUOTATION_MARK_END},
        {BACKSPACE, codePoint, CODEPOINT},
        {BACKSPACE, reverseSolidus, REVERSE_SOLIDUS_START},
        {FORMFEED, quote, QUOTATION_MARK_END},
        {FORMFEED, codePoint, CODEPOINT},
        {FORMFEED, reverseSolidus, REVERSE_SOLIDUS_START},
        {LINEFEED, quote, QUOTATION_MARK_END},
        {LINEFEED, codePoint, CODEPOINT},
        {LINEFEED, reverseSolidus, REVERSE_SOLIDUS_START},
        {CARRIAGE_RETURN, quote, QUOTATION_MARK_END},
        {CARRIAGE_RETURN, codePoint, CODEPOINT},
        {CARRIAGE_RETURN, reverseSolidus, REVERSE_SOLIDUS_START},
        {HORIZONTAL_TAB, quote, QUOTATION_MARK_END},
        {HORIZONTAL_TAB, codePoint, CODEPOINT},
        {HORIZONTAL_TAB, reverseSolidus, REVERSE_SOLIDUS_START},
        {UNICODE, quote, QUOTATION_MARK_END},
        {UNICODE, codePoint, CODEPOINT},
        {UNICODE, reverseSolidus, REVERSE_SOLIDUS_START},
};

int parser_sm_string_define()
{
    StateMachine *sm = parser_sm_create();

    // Create states

    // QUOTATION_MARK_START
    parser_sm_create
}

// string state machine

// states
// QUOTATION_MARK_START
// QUOTATION_MARK_END
// CODEPOINT
// REVERSE_SOLIDUS_START
// QUOTATION_MARK
// REVERSE_SOLIDUS
// SOLIDUS
// BACKSPACE
// FORMFEED
// LINEFEED
// CARRIAGE_RETURN
// HORIZONTAL_TAB
// UNICODE
// HEX_CHARACTER_1
// HEX_CHARACTER_2
// HEX_CHARACTER_3
// HEX_CHARACTER_4

// transitions
// QUOTATION_MARK_START
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// QUOTATION_MARK_END
// no transitions, string has been detected

// CODEPOINT
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// REVERSE_SOLIDUS_START
// " -> QUOTATION_MARK
// \ -> REVERSE_SOLIDUS
// / -> SOLIDUS
// b -> BACKSPACE
// f -> FORMFEED
// n -> LINEFEED
// r -> CARRIAGE_RETURN
// t -> HORIZONTAL_TAB
// u -> UNICODE

// QUOTATION_MARK
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// REVERSE_SOLIDUS
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// SOLIDUS
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// BACKSPACE
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// FORMFEED
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// LINEFEED
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// CARRIAGE_RETURN
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// HORIZONTAL_TAB
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

// UNICODE
// hex char -> HEX_CHARACTER_1

// HEX_CHARACTER_1
// hex char -> HEX_CHARACTER_2

// HEX_CHARACTER_2
// hex char -> HEX_CHARACTER_3

// HEX_CHARACTER_3
// hex char -> HEX_CHARACTER_4

// HEX_CHARACTER_4
// - " -> QUOTATION_MARK_END
// - character except " and \ and control characters -> CODEPOINT
// - \ -> REVERSE_SOLIDUS_START

int parser_sm_string_define()
{
    StateMachine
}