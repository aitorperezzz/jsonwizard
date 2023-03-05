#include "string_type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

String *stringCreate(void)
{
    String *result = malloc(sizeof(String));
    if (result == NULL)
    {
        printf("ERROR: memory error, could not reserve memory for new string");
        return NULL;
    }
    result->buffer = NULL;
    result->capacity = 0;
    result->length = 0;
    return result;
}

String *stringCreateFromLiteral(const char *literal)
{
    String *result = stringCreate();
    const size_t originalLength = strlen(literal);
    void *tmp = malloc(originalLength + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    strcpy(result->buffer, literal);
    result->capacity = originalLength + 1;
    result->length = originalLength;
    return result;
}

size_t stringLength(const String *string)
{
    return string->length;
}

char stringGet(const String *string, const size_t index)
{
    return string->buffer[index];
}

ResultCode stringCopy(String *destination, const String *origin)
{
    if (destination == NULL || origin == NULL)
    {
        return JSON_MEMORY_ERROR;
    }
    return stringCopyFromBuffer(destination, origin->buffer, origin->length);
}

ResultCode stringCopyFromBuffer(String *destination, const char *origin, const size_t size)
{
    if (destination == NULL || origin == NULL)
    {
        return JSON_MEMORY_ERROR;
    }

    // Create a string with the data that has been provided
    String *addition = stringCreate();
    void *tmp = malloc(size + 1);
    if (tmp == NULL)
    {
        return JSON_MEMORY_ERROR;
    }
    addition->buffer = tmp;
    memcpy(addition->buffer, origin, size);
    addition->buffer[size] = '\0';
    addition->capacity = size + 1;
    addition->length = size;

    // Delete contents in original string
    stringFree(destination);

    // Join both strings in place
    if (stringJoinInPlace(destination, addition) != JSON_OK)
    {
        return JSON_ERROR;
    }

    return JSON_OK;
}

int stringCompare(const String *string1, const String *string2)
{
    return strcmp(stringBuffer(string1), stringBuffer(string2));
}

ResultCode stringJoinInPlace(String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        printf("ERROR: strings provided are NULL");
        return JSON_MEMORY_ERROR;
    }

    // Check, if second string is empty, do nothing
    if (string2->length == 0)
    {
        return JSON_OK;
    }

    // Check string 1 has enough capacity
    if (string1->capacity + string1->length < string2->length)
    {
        void *tmpBuffer = realloc(string1->buffer, string1->length + string2->length + 1);
        if (tmpBuffer)
        {
            printf("ERROR: could not reserve memory");
            return JSON_MEMORY_ERROR;
        }
        string1->buffer = tmpBuffer;
    }

    // Copy the second string into the first one
    memcpy(string1->buffer + string1->length, string2->buffer, string2->length);
    size_t finalLength = string1->length + string2->length;
    string1->buffer[finalLength] = '\0';
    string1->length = finalLength;
    string1->capacity = finalLength + 1;
    return JSON_OK;
}

String *stringJoin(const String *string1, const String *string2)
{
    String *result = stringCreate();
    stringJoinInPlace(result, string2);
    return result;
}

void stringFree(String *string)
{
    free(string->buffer);
    string->buffer = NULL;
    string->length = 0;
    string->capacity = 0;
}

ResultCode stringReserve(String *string, const size_t additionalCapacity)
{
    if (string == NULL)
    {
        printf("ERROR: string passed is NULL");
        return JSON_MEMORY_ERROR;
    }
    char *tmp = realloc(string->buffer, string->capacity + additionalCapacity);
    if (tmp == NULL)
    {
        printf("ERROR: could not add more capacity for string. Leaving as is");
        return JSON_MEMORY_ERROR;
    }
    string->buffer = tmp;
    string->capacity = string->capacity + additionalCapacity;
    return JSON_OK;
}

const char *stringBuffer(const String *string)
{
    return string->buffer;
}