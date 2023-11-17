#ifndef TYPES_STRING_H
#define TYPES_STRING_H

#include <stdlib.h>

#include "utils.h"

/// @brief String
typedef struct String_st
{
    char *buffer; // Internal C string
    int length;   // Number of characters
    int capacity; // Number of bytes reserved in the internal buffer
} String;

/// @brief Create an empty string
/// @retval Pointer to a string structure initialised as an empty string
/// @retval NULL if a problem was encountered
String *types_string_create(void);

/// @brief Create a string from an already existing C string
/// @param literal Pointer to an already existing C string
/// @retval Pointer to a string that holds the same values as the original C string
/// @retval NULL if a problem was encountered
String *types_string_create_from_literal(const char *literal);

/// @brief Create a string from a provided buffer with a specified size
/// @param origin Original buffer
/// @param size Number of characters to copy from the buffer
/// @retval String that holds the same characters as the provided buffer
/// @retval NULL if a problem was encountered
String *types_string_create_from_buffer(const char *origin, const size_t size);

/// @brief Return the number of characters in the provided string
/// @param string String
/// @return Number of characters in the string
size_t types_string_length(const String *string);

/// @brief Return the internal NULL-terminated C string
/// @param string String
/// @retval Internal char * buffer of the string
/// @retval NULL if a problem was encountered
const char *types_string_c_str(const String *string);

/// @brief Return the character in the string at the specified index.
/// This function does not perform range checking.
/// @param string String
/// @param index Index of the character to retrieve. Be careful to provide an index in bounds
/// @return The required character
char types_string_at(const String *string, const size_t index);

/// @brief Return an exact copy of the provided string
/// @param original An already existing string
/// @retval Copy of the original string
/// @retval NULL if a problem was encountered
String *types_string_copy(const String *original);

/// @brief Compare the provided strings with the same return value as strcmp
/// @param string1 Frist string to compare
/// @param string2 Second string to compare
/// @retval Same return value as strcmp
int types_string_compare(const String *string1, const String *string2);

/// @brief Join the second string provied to the first one
/// @param string1 String where the final sum of the two will be stored
/// @param string2 String to join to the first one
/// @return Result code
ResultCode types_string_join_in_place(String *string1, const String *string2);

/// @brief Join two strings and return the result
/// @param string1 First string to join
/// @param string2 Second string to join
/// @retval The string that is the concatenation of the two
/// @retval NULL if a problem was encountered
String *types_string_join(const String *string1, const String *string2);

/// @brief Reserve, ahead of time, a number of bytes for the string, to prevent
/// reallocations when the size of the final string is known
/// @param string String
/// @param capacity Final requested capacity of the string in bytes
/// @return Result code
ResultCode types_string_reserve(String *string, const size_t capacity);

/// @brief Free all the internal memory used by the string
/// @param string Raw pointer that in reality points to a String type
/// @return Result code
ResultCode types_string_free(void *string);

#endif