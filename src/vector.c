#include "vector.h"

#include <string.h>
#include <stdio.h>

#include "string_type.h"

Vector *vectorCreate(const VectorType type)
{
    Vector *result = malloc(sizeof(Vector));
    if (result == NULL)
    {
        printf("ERROR: could not reserve memory for vector");
        return NULL;
    }
    result->type = type;
    result->size = 0;
    result->capacity = 0;
    result->data = NULL;
    switch (result->type)
    {
    case VECTOR_TYPE_STRING:
        result->elementSize = sizeof(struct String *);
        break;
    default:
        return NULL;
    }
    return result;
}

const size_t vectorSize(const Vector *vector)
{
    return vector->size;
}

ResultCode vectorAdd(Vector *vector, const void *data, const size_t dataSize)
{
    // Initial checks
    if (vector == NULL)
    {
        printf("ERROR: vector provided is NULL");
        return JSON_MEMORY_ERROR;
    }
    if (data == NULL)
    {
        printf("ERROR: data provided to add to vector is NULL");
        return JSON_MEMORY_ERROR;
    }

    // Check the data size provided matches the expected element size
    if (dataSize != vector->elementSize)
    {
        printf("ERROR: data provided does not have the expected size. Will not add");
        return JSON_MEMORY_ERROR;
    }

    // Check if I need to reserve more memory
    if (vector->size == vector->capacity)
    {
        void *tmp = realloc(vector->data, vector->capacity + vector->elementSize);
        if (tmp == NULL)
        {
            return JSON_MEMORY_ERROR;
        }
        vector->data = tmp;
        vector->capacity += 1;
    }

    // Add element to the end of vector
    memcpy(vector->data + vector->elementSize * vector->size, data, vector->elementSize);
    vector->size += 1;
    return JSON_OK;
}

ResultCode vectorClear(Vector *vector)
{
    return JSON_OK;
}

ResultCode vectorFree(Vector *vector)
{
    return JSON_OK;
}

void *vectorGet(const Vector *vector, const size_t index)
{
    if (index > vector->size)
    {
        printf("ERROR: cannot access index %zu of vector with length %zu", index, vector->size);
        return NULL;
    }
    return vector->data + index * vector->elementSize;
}

void vectorSet(Vector *vector, const size_t index, const void *data)
{
}
void vectorErase(Vector *vector, const size_t index)
{
}