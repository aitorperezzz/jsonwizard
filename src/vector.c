#include <string.h>
#include <stdio.h>

#include "vector.h"

Vector *vector_create(const size_t elementSize, ResultCode (*freeCallback)(void *))
{
    if (freeCallback == NULL)
    {
        printf("Please provide a non-NULL callback function to free elements of vector\n");
        return NULL;
    }
    Vector *result = malloc(sizeof(Vector));
    if (result == NULL)
    {
        return NULL;
    }
    result->size = 0;
    result->capacity = 0;
    result->data = NULL;
    result->elementSize = elementSize;
    result->freeCallback = freeCallback;
    return result;
}

size_t vector_size(const Vector *vector)
{
    if (vector == NULL)
    {
        printf("Attempt to get size of NULL vector\n");
        return 0;
    }
    return vector->size;
}

ResultCode vector_push(Vector *vector, const void *data)
{
    // Initial checks
    if (vector == NULL)
    {
        printf("Cannot push to NULL vector\n");
        return CODE_MEMORY_ERROR;
    }
    if (data == NULL)
    {
        printf("Cannot push NULL data to vector\n");
        return CODE_MEMORY_ERROR;
    }

    // Check if the buffer has not been allocated yet
    if (vector->data == NULL)
    {
        vector->data = malloc(vector->elementSize);
        if (vector->data == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        vector->capacity += 1;
    }

    // Check if more memory is needed
    if (vector->size == vector->capacity)
    {
        void *tmp = realloc(vector->data, vector->elementSize * (vector->capacity + 1));
        if (tmp == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        vector->data = tmp;
        vector->capacity += 1;
    }

    // Add element to the end of vector
    Iterator end = vector_end(vector);
    memcpy(end.pointer, data, vector->elementSize);
    vector->size += 1;
    return CODE_OK;
}

ResultCode vector_clear(Vector *vector)
{
    if (vector == NULL)
    {
        printf("Cannot clear a NULL vector\n");
        return CODE_MEMORY_ERROR;
    }
    // Free the memory in use at each index in the vector
    for (size_t i = 0, n = vector->size; i < n; i++)
    {
        if (vector->freeCallback(vector_get(vector, i)) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }

    // All the internal elements have been freed, so clear the vector
    memset(vector->data, '\0', vector->size * vector->elementSize);
    vector->size = 0;

    return CODE_OK;
}

ResultCode vector_free(Vector *vector)
{
    if (vector == NULL)
    {
        printf("Cannot free a NULL vector\n");
        return CODE_MEMORY_ERROR;
    }
    if (vector_clear(vector) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    free(vector->data);
    vector->size = 0;
    vector->capacity = 0;
    vector->elementSize = 0;
    return CODE_OK;
}

void *vector_get(const Vector *vector, const size_t index)
{
    if (vector == NULL)
    {
        printf("Cannot get elements of NULL vector\n");
        return NULL;
    }
    if (index >= vector->size)
    {
        printf("Cannot access element at index %lu\n", index);
        return NULL;
    }
    return vector->data + index * vector->elementSize;
}

ResultCode vector_set(Vector *vector, const size_t index, const void *data)
{
    if (vector == NULL)
    {
        printf("Cannot set elements of NULL vector\n");
        return CODE_MEMORY_ERROR;
    }
    if (data == NULL)
    {
        printf("Cannot set NULL data into vector\n");
        return CODE_MEMORY_ERROR;
    }
    if (index >= vector->size)
    {
        printf("Cannot access element at index %lu\n", index);
        return CODE_LOGIC_ERROR;
    }
    memcpy(vector->data + index * vector->elementSize, data, vector->elementSize);
    return CODE_OK;
}

Iterator vector_begin(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        printf("Cannot create begin iterator\n");
        return iterator_invalidIterator();
    }
    return iterator_create(vector->data, vector->elementSize);
}

Iterator vector_end(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        printf("Cannot create end iterator\n");
        return iterator_invalidIterator();
    }
    return iterator_create(vector->data + vector->size * vector->elementSize, vector->elementSize);
}

ResultCode vector_erase(Vector *vector, Iterator first, Iterator last)
{
    if (first.pointer >= last.pointer)
    {
        return CODE_OK;
    }

    Iterator end = vector_end(vector);
    const size_t distance = iterator_distance(last, end);
    Iterator writeIterator = first;
    Iterator readIterator = last;
    for (size_t i = 0; i < distance; i++)
    {
        if (vector->freeCallback(iterator_get(writeIterator)) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
        memcpy(iterator_get(writeIterator), iterator_get(readIterator), vector->elementSize);
        memset(iterator_get(readIterator), '\0', vector->elementSize);
        readIterator = iterator_increase(readIterator, 1);
        writeIterator = iterator_increase(writeIterator, 1);
    }
    vector->size -= iterator_distance(first, last);

    return CODE_OK;
}