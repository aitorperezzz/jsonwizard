#include <stdbool.h>
#include <ctype.h>

#include "read_sm_callbacks.h"

bool callback_quote(const char c)
{
    return c == '\"';
}

bool callback_code_point(const char c)
{
    return c != '\"' && c != '\\' && c >= 31;
}

bool callback_reverse_solidus(const char c)
{
    return c == '\\';
}

bool callback_solidus(const char c)
{
    return c == '/';
}

bool callback_backspace(const char c)
{
    return c == 'b';
}

bool callback_form_feed(const char c)
{
    return c == 'f';
}

bool callback_line_feed(const char c)
{
    return c == 'n';
}

bool callback_carriage_return(const char c)
{
    return c == 'r';
}

bool callback_horizontal_tab(const char c)
{
    return c == 't';
}

bool callback_unicode(const char c)
{
    return c == 'u';
}

bool callback_zero(const char c)
{
    return c == '0';
}

bool callback_minus(const char c)
{
    return c == '-';
}

bool callback_plus(const char c)
{
    return c == '+';
}

bool callback_digit_1to9(const char c)
{
    return isdigit(c) && !callback_zero(c);
}

bool callback_digit(const char c)
{
    return isdigit(c);
}

bool callback_dot(const char c)
{
    return c == '.';
}

bool callback_exponent_e(const char c)
{
    return c == 'E' || c == 'e';
}

bool callback_t(const char c)
{
    return c == 't';
}

bool callback_r(const char c)
{
    return c == 'r';
}

bool callback_u(const char c)
{
    return c == 'u';
}

bool callback_e(const char c)
{
    return c == 'e';
}

bool callback_f(const char c)
{
    return c == 'f';
}

bool callback_a(const char c)
{
    return c == 'a';
}

bool callback_l(const char c)
{
    return c == 'l';
}

bool callback_s(const char c)
{
    return c == 's';
}

bool callback_n(const char c)
{
    return c == 'n';
}

bool callback_whitespace(const char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}