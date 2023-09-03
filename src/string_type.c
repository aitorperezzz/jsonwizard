#include "string_type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

String *string_create(void)
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

String *string_createFromLiteral(const char *literal)
{
    if (literal == NULL)
    {
        return NULL;
    }

    // Create a non initialised string
    String *result = malloc(sizeof(String));
    if (result == NULL)
    {
        return NULL;
    }
    result->buffer = NULL;
    result->length = 0;
    result->capacity = 0;

    // Directly reserve the necessary memory
    const size_t originalLength = strlen(literal);
    void *tmp = malloc(originalLength + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    memcpy(result->buffer, literal, originalLength);
    result->buffer[originalLength] = '\0';
    result->length = originalLength;
    result->capacity = originalLength + 1;
    return result;
}

String *string_createFromBuffer(const char *origin, const size_t size)
{
    if (origin == NULL)
    {
        return NULL;
    }

    String *destination = string_create();
    if (destination == NULL)
    {
        return NULL;
    }

    // Alloc memory for the buffer
    void *tmp = malloc(size + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    destination->buffer = tmp;
    memcpy(destination->buffer, origin, size);
    destination->buffer[size] = '\0';
    destination->length = size;
    destination->capacity = size + 1;
    return destination;
}

size_t string_length(const String *string)
{
    if (string == NULL)
    {
        return 0;
    }
    return string->length;
}

const char *string_cStr(const String *string)
{
    if (string == NULL)
    {
        return NULL;
    }
    return string->buffer;
}

char string_at(const String *string, const size_t index)
{
    return string->buffer[index];
}

String *string_copy(const String *original)
{
    if (original == NULL)
    {
        return NULL;
    }
    return string_createFromBuffer(original->buffer, original->length);
}

int string_compare(const String *string1, const String *string2)
{
    return strcmp(string_cStr(string1), string_cStr(string2));
}

ResultCode string_joinInPlace(String *string1, const String *string2)
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

String *string_join(const String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        return NULL;
    }
    String *result = string_createFromBuffer(string1->buffer, string1->length);
    string_joinInPlace(result, string2);
    return result;
}

ResultCode string_reserve(String *string, const size_t capacity)
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

ResultCode string_free(void *string)
{
    if (string == NULL)
    {
        return CODE_OK;
    }
    String *mystring = (String *)string;
    if (mystring->buffer != NULL)
    {
        free(mystring->buffer);
    }
    mystring->buffer = NULL;
    mystring->length = 0;
    mystring->capacity = 0;
    return CODE_OK;
}
