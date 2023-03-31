#ifndef PARSE_H
#define PARSE_H

#include "utils.h"
#include "string_type.h"

ResultCode parseNumber(const String *data, double *value);
ResultCode parseBoolean(const String *data, Boolean *value);

#endif