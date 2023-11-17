#ifndef PARSER_CALLBACKS_H
#define PARSER_CALLBACKS_H

bool quote(const char c);
bool code_point(const char c);
bool reverse_solidus(const char c);
bool solidus(const char c);
bool backspace(const char c);
bool form_feed(const char c);
bool line_feed(const char c);
bool carriage_return(const char c);
bool horizontal_tab(const char c);
bool unicode(const char c);

#endif