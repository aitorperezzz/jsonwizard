#include "string_type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static String *stringCreateNonInitialised();

String *stringCreateNonInitialised()
{
    String *result = malloc(sizeof(String));
    if (result == NULL)
    {
        return NULL;
    }
    result->buffer = NULL;
    result->length = 0;
    result->capacity = 0;
    return result;
}

String *stringCreate(void)
{
    // Reserve memory for the structure itself
    String *result = malloc(sizeof(String));
    if (result == NULL)
    {
        return NULL;
    }
    // Initialise as an empty string
    void *tmp = malloc(1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    result->buffer[0] = '\0';
    result->capacity = 1;
    result->length = 0;
    return result;
}

String *stringCreateFromLiteral(const char *literal)
{
    String *result = stringCreateNonInitialised();
    const size_t originalLength = strlen(literal);
    void *tmp = malloc(originalLength + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    memcpy(result->buffer, literal, originalLength);
    result->buffer[originalLength] = '\0';
    result->capacity = originalLength + 1;
    result->length = originalLength;
    return result;
}

size_t stringGetLength(const String *string)
{
    return string->length;
}

char stringGetChar(const String *string, const size_t index)
{
    return string->buffer[index];
}

ResultCode stringCopy(String *destination, const String *origin)
{
    if (destination == NULL || origin == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    return stringCopyFromBuffer(destination, origin->buffer, origin->length);
}

ResultCode stringCopyFromBuffer(String *destination, const char *origin, const size_t size)
{
    if (destination == NULL || origin == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Reset the destination string
    stringReset(destination);

    // Alloc memory for the buffer
    void *tmp = malloc(size + 1);
    if (tmp == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    destination->buffer = tmp;
    memcpy(destination->buffer, origin, size);
    destination->buffer[size] = '\0';
    destination->length = size;
    destination->capacity = size + 1;
    return CODE_OK;
}

int stringCompare(const String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        return -1;
    }
    if (stringGetBuffer(string1) == NULL || stringGetBuffer(string2) == NULL)
    {
        return -1;
    }
    return strcmp(stringGetBuffer(string1), stringGetBuffer(string2));
}

ResultCode stringJoinInPlace(String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Check, if second string is empty, do nothing
    if (string2->length == 0)
    {
        return CODE_OK;
    }

    // Check string 1 has enough capacity
    size_t memoryNeeded = string1->length + string2->length + 1;
    if (string1->capacity < memoryNeeded)
    {
        void *tmp = realloc(string1->buffer, memoryNeeded);
        if (tmp == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        string1->buffer = tmp;
    }

    // Copy the second string into the first one
    memcpy(string1->buffer + string1->length, string2->buffer, string2->length);
    size_t finalLength = string1->length + string2->length;
    string1->buffer[finalLength] = '\0';
    string1->length = finalLength;
    string1->capacity = finalLength + 1;
    return CODE_OK;
}

String *stringJoin(const String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        return NULL;
    }
    String *result = stringCreateNonInitialised();
    stringCopyFromBuffer(result, string1->buffer, string1->length);
    stringJoinInPlace(result, string2);
    return result;
}

void stringReset(String *string)
{
    if (string == NULL)
    {
        return;
    }
    if (string->buffer != NULL)
    {
        free(string->buffer);
    }
    string->buffer = NULL;
    string->length = 0;
    string->capacity = 0;
}

ResultCode stringReserve(String *string, const size_t capacity)
{
    if (string == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    // Weird case where the user would want less capacity than currently?
    // Probably should output a warning, but for the moment I ignore it
    if (capacity <= string->capacity)
    {
        return CODE_OK;
    }
    char *tmp = realloc(string->buffer, capacity);
    if (tmp == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    string->buffer = tmp;
    string->capacity = capacity;
    return CODE_OK;
}

const char *stringGetBuffer(const String *string)
{
    return string->buffer;
}