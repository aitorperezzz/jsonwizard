#ifndef READ_SM_CALLBACKS_H
#define READ_SM_CALLBACKS_H

bool callback_quote(const char c);
bool callback_code_point(const char c);
bool callback_reverse_solidus(const char c);
bool callback_solidus(const char c);
bool callback_backspace(const char c);
bool callback_form_feed(const char c);
bool callback_line_feed(const char c);
bool callback_carriage_return(const char c);
bool callback_horizontal_tab(const char c);
bool callback_unicode(const char c);

bool callback_zero(const char c);
bool callback_minus(const char c);
bool callback_plus(const char c);
bool callback_digit_1to9(const char c);
bool callback_digit(const char c);
bool callback_dot(const char c);
bool callback_exponent_e(const char c);

bool callback_t(const char c);
bool callback_r(const char c);
bool callback_u(const char c);
bool callback_e(const char c);

bool callback_f(const char c);
bool callback_a(const char c);
bool callback_l(const char c);
bool callback_s(const char c);

bool callback_n(const char c);

bool callback_whitespace(const char c);

#endif