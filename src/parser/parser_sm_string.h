#ifndef PARSER_SM_STRING_H
#define PARSER_SM_STRING_H

#include "parser_sm.h"

StateMachine *parser_sm_string_define();

#endif

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