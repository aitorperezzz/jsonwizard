#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "types_string.h"

String *types_string_create(void)
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

String *types_string_create_from_literal(const char *literal)
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
    const size_t original_length = strlen(literal);
    void *tmp = malloc(original_length + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    memcpy(result->buffer, literal, original_length);
    result->buffer[original_length] = '\0';
    result->length = original_length;
    result->capacity = original_length + 1;
    return result;
}

String *types_string_create_from_buffer(const char *origin, const size_t size)
{
    if (origin == NULL)
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

    // Alloc memory for the buffer
    void *tmp = malloc(size + 1);
    if (tmp == NULL)
    {
        return NULL;
    }
    result->buffer = tmp;
    memcpy(result->buffer, origin, size);
    result->buffer[size] = '\0';
    result->length = size;
    result->capacity = size + 1;
    return result;
}

size_t types_string_length(const String *string)
{
    if (string == NULL)
    {
        return 0;
    }
    return string->length;
}

const char *types_string_c_str(const String *string)
{
    if (string == NULL)
    {
        return NULL;
    }
    return string->buffer;
}

char types_string_at(const String *string, const size_t index)
{
    return string->buffer[index];
}

String *types_string_copy(const String *original)
{
    if (original == NULL)
    {
        return NULL;
    }
    return types_string_create_from_buffer(original->buffer, original->length);
}

int types_string_compare(const String *string1, const String *string2)
{
    return strcmp(types_string_c_str(string1), types_string_c_str(string2));
}

ResultCode types_string_join_in_place(String *string1, const String *string2)
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
    size_t memory_needed = string1->length + string2->length + 1;
    if (string1->capacity < memory_needed)
    {
        void *tmp = realloc(string1->buffer, memory_needed);
        if (tmp == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        string1->buffer = tmp;
    }

    // Copy the second string into the first one
    memcpy(string1->buffer + string1->length, string2->buffer, string2->length);
    size_t final_length = string1->length + string2->length;
    string1->buffer[final_length] = '\0';
    string1->length = final_length;
    string1->capacity = final_length + 1;
    return CODE_OK;
}

String *types_string_join(const String *string1, const String *string2)
{
    if (string1 == NULL || string2 == NULL)
    {
        return NULL;
    }
    String *result = types_string_create_from_buffer(string1->buffer, string1->length);
    types_string_join_in_place(result, string2);
    return result;
}

ResultCode types_string_reserve(String *string, const size_t capacity)
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

ResultCode types_string_free(void *string)
{
    if (string == NULL)
    {
        return CODE_OK;
    }
    String *my_string = (String *)string;
    if (my_string->buffer != NULL)
    {
        free(my_string->buffer);
    }
    my_string->buffer = NULL;
    my_string->length = 0;
    my_string->capacity = 0;
    return CODE_OK;
}
