#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include <stdlib.h>

#include "utils.h"

typedef struct String_st
{
    char *buffer;
    int length;
    int capacity;
} String;

String *stringCreate(void);
String *stringCreateFromLiteral(const char *literal);
size_t stringLength(const String *string);
char stringGet(const String *string, const size_t index);
ResultCode stringCopy(String *destination, const String *origin);
ResultCode stringCopyFromBuffer(String *destination, const char *origin, const size_t size);
int stringCompare(const String *string1, const String *string2);
ResultCode stringJoinInPlace(String *string1, const String *string2);
String *stringJoin(const String *string1, const String *string2);
void stringFree(String *string);
ResultCode stringReserve(String *string, const size_t capacity);
const char *stringBuffer(const String *string);

#endif