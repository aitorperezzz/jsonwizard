#include <string.h>

#include "vector.h"

Vector *vector_create(const size_t elementSize, ResultCode (*freeCallback)(void *))
{
    if (freeCallback == NULL || elementSize == 0)
    {
        return NULL;
    }

    // Allocate memory and initialise with default values
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
        return 0;
    }
    return vector->size;
}

ResultCode vector_push(Vector *vector, const void *data)
{
    if (vector == NULL || data == NULL)
    {
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
        return CODE_MEMORY_ERROR;
    }

    // Free the memory in use at each index in the vector
    for (size_t i = 0, n = vector->size; i < n; i++)
    {
        if (vector->freeCallback(vector_at(vector, i)) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }

    // All the internal elements have been freed, so fill the vector with zeros but keep the memory reserved
    memset(vector->data, '\0', vector->size * vector->elementSize);
    vector->size = 0;
    return CODE_OK;
}

ResultCode vector_free(Vector *vector)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // First free the memory used by each of the elements
    if (vector_clear(vector) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }

    // Free the internal buffer
    free(vector->data);

    // Reset variables
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->elementSize = 0;
    return CODE_OK;
}

void *vector_at(const Vector *vector, const size_t index)
{
    if (vector == NULL || index >= vector->size)
    {
        return NULL;
    }
    return vector->data + index * vector->elementSize;
}

ResultCode vector_set(Vector *vector, const size_t index, const void *data)
{
    if (vector == NULL || data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (index >= vector->size)
    {
        return CODE_LOGIC_ERROR;
    }
    memcpy(vector_at(vector, index), data, vector->elementSize);
    return CODE_OK;
}

Iterator vector_begin(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        return iterator_invalid();
    }
    return iterator_create(vector->data, vector->elementSize);
}

Iterator vector_end(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        return iterator_invalid();
    }
    return iterator_create(vector->data + vector->size * vector->elementSize, vector->elementSize);
}

ResultCode vector_erase(Vector *vector, Iterator first, Iterator last)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (first.pointer >= last.pointer)
    {
        return CODE_OK;
    }

    // Compute the distance between last and end, as these will be the elements to move
    Iterator end = vector_end(vector);
    size_t distance;
    if (iterator_distance(last, end, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    // Write on the first iterator the contents of the last iterator, advance, and so on
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

    // Reduce the size of the vector accordingly
    if (iterator_distance(first, last, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }
    vector->size -= distance;
    return CODE_OK;
}

ResultCode vector_insert(Vector *vector, Iterator first, Iterator last, Iterator destination)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Compute the number of new elements
    size_t numElementsAdded = 0;
    if (iterator_distance(first, last, &numElementsAdded) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    // Compute the number of elements that will be displaced
    size_t numElementsDisplaced = 0;
    if (iterator_distance(destination, vector_end(vector), &numElementsDisplaced) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }

    // Reallocate the vector to accomodate the new elements
    void *tmp = realloc(vector->data, (vector->capacity + numElementsAdded) * vector->elementSize);
    if (tmp == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    vector->data = tmp;
    vector->size += numElementsAdded;
    vector->capacity += numElementsAdded;

    // Move all the elements between destination and end, to the end of the vector
    // The last element is moved first to prevent overwrites. Then the iterator progresses down
    Iterator readIterator, writeIterator;
    readIterator = iterator_increase(destination, numElementsDisplaced - 1);
    writeIterator = iterator_increase(readIterator, numElementsAdded);
    for (size_t i = 0; i < numElementsDisplaced; i++)
    {
        memcpy(iterator_get(writeIterator), iterator_get(readIterator), vector->elementSize);
        readIterator = iterator_decrease(readIterator, 1);
        writeIterator = iterator_decrease(writeIterator, 1);
    }

    // Write the new elements
    readIterator = first;
    writeIterator = destination;
    for (size_t i = 0; i < numElementsAdded; i++)
    {
        memcpy(iterator_get(writeIterator), iterator_get(readIterator), vector->elementSize);
        readIterator = iterator_increase(readIterator, 1);
        writeIterator = iterator_increase(writeIterator, 1);
    }

    return CODE_OK;
}

bool vector_empty(const Vector *vector)
{
    if (vector == NULL)
    {
        return true;
    }
    return vector_size(vector) == 0;
}