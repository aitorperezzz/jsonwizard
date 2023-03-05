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

/// @brief Create a string. This method always needs to be called when creating a string
/// @return Pointer to a string structure
String *stringCreate(void);

/// @brief Create a string structure from an already existing C string
/// @param literal Already existing C string
/// @return Pointer to a string structure
String *stringCreateFromLiteral(const char *literal);

/// @brief Returns the size of the internal string in a string structure
/// @param string String structure
/// @return Length of the string
size_t stringGetLength(const String *string);

/// @brief Returns the internal buffer of the string
/// @param string String structure
/// @return Internal char * buffer of the string
const char *stringGetBuffer(const String *string);

/// @brief Access a character in a string, by index
/// @param string String structure
/// @param index Index of the character to retrieve
/// @return The required character
char stringGetChar(const String *string, const size_t index);

/// @brief Copies the provided string to another, already existing, string
/// @param destination Already existing destination string
/// @param origin Already existing origin string
/// @return Result code
ResultCode stringCopy(String *destination, const String *origin);

/// @brief Copy a string from a buffer
/// @param destination Already existing destination string
/// @param origin Original buffer
/// @param size NUmber of characters to copy from the buffer
/// @return Result code
ResultCode stringCopyFromBuffer(String *destination, const char *origin, const size_t size);

/// @brief Compare the provided strings, essentially wrapping strcmp
/// @param string1 Frist string to compare
/// @param string2 Second string to compare
/// @return Value returned by strcmp
int stringCompare(const String *string1, const String *string2);

/// @brief Join two strings in place, place the joined string on the first argument
/// @param string1 String where the final sum of the two will be stored
/// @param string2 String to join to the first one
/// @return Result code
ResultCode stringJoinInPlace(String *string1, const String *string2);

/// @brief Join two strings and return the result
/// @param string1 First string to join
/// @param string2 Second string to join
/// @return Resulting string
String *stringJoin(const String *string1, const String *string2);

/// @brief Resets the internal variables of the string, prior to call free() or to create a new one
/// @param string String structure
void stringReset(String *string);

/// @brief Reserve, ahead of time, a number of bytes for the string, to prevent many
/// reallocations when the size of the final string is known
/// @param string String structure
/// @param capacity Number of bytes with the final capacity of the string
/// @return Result code
ResultCode stringReserve(String *string, const size_t capacity);

#endif