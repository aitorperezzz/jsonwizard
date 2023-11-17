#include <stdbool.h>

#include "parser/parser_callbacks.h"

bool quote(const char c)
{
    return c == '\"';
}

bool code_point(const char c)
{
    return c != (char)'\"' && c != (char)'\\' && c >= 31;
}

bool reverse_solidus(const char c)
{
    return c == (char)'\\';
}

bool solidus(const char c)
{
    return c == (char)'\\';
}

bool backspace(const char c)
{
    return c == (char)'\\';
}

bool form_feed(const char c)
{
    return c == (char)'\\';
}

bool line_feed(const char c)
{
    return c == (char)'\\';
}

bool carriage_return(const char c)
{
    return c == (char)'\\';
}

bool horizontal_tab(const char c)
{
    return c == (char)'\\';
}

bool unicode(const char c)
{
    return c == (char)'\\';
}