#include <string.h>

#include "types_vector.h"

Vector *types_vector_create(const size_t element_size, ResultCode (*free_callback)(void *))
{
    if (free_callback == NULL || element_size == 0)
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
    result->element_size = element_size;
    result->free_callback = free_callback;
    return result;
}

size_t types_vector_size(const Vector *vector)
{
    if (vector == NULL)
    {
        return 0;
    }
    return vector->size;
}

ResultCode types_vector_push(Vector *vector, const void *data)
{
    if (vector == NULL || data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Check if the buffer has not been allocated yet
    if (vector->data == NULL)
    {
        vector->data = malloc(vector->element_size);
        if (vector->data == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        vector->capacity += 1;
    }

    // Check if more memory is needed
    if (vector->size == vector->capacity)
    {
        void *tmp = realloc(vector->data, vector->element_size * (vector->capacity + 1));
        if (tmp == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
        vector->data = tmp;
        vector->capacity += 1;
    }

    // Add element to the end of vector
    Iterator end = types_vector_end(vector);
    memcpy(end.pointer, data, vector->element_size);
    vector->size += 1;
    return CODE_OK;
}

ResultCode types_vector_clear(Vector *vector)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Free the memory in use at each index in the vector
    for (size_t i = 0, n = vector->size; i < n; i++)
    {
        if (vector->free_callback(types_vector_at(vector, i)) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }

    // All the internal elements have been freed, so fill the vector with zeros but keep the memory reserved
    memset(vector->data, '\0', vector->size * vector->element_size);
    vector->size = 0;
    return CODE_OK;
}

ResultCode types_vector_free(Vector *vector)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // First free the memory used by each of the elements
    if (types_vector_clear(vector) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }

    // Free the internal buffer
    free(vector->data);

    // Reset variables
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = 0;
    return CODE_OK;
}

void *types_vector_at(const Vector *vector, const size_t index)
{
    if (vector == NULL || index >= vector->size)
    {
        return NULL;
    }
    return vector->data + index * vector->element_size;
}

ResultCode types_vector_set(Vector *vector, const size_t index, const void *data)
{
    if (vector == NULL || data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (index >= vector->size)
    {
        return CODE_LOGIC_ERROR;
    }
    memcpy(types_vector_at(vector, index), data, vector->element_size);
    return CODE_OK;
}

Iterator types_vector_begin(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        return types_iterator_invalid();
    }
    return types_iterator_create(vector->data, vector->element_size);
}

Iterator types_vector_end(const Vector *vector)
{
    if (vector == NULL || vector->data == NULL)
    {
        return types_iterator_invalid();
    }
    return types_iterator_create(vector->data + vector->size * vector->element_size, vector->element_size);
}

ResultCode types_vector_erase(Vector *vector, Iterator first, Iterator last)
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
    Iterator end = types_vector_end(vector);
    size_t distance;
    if (types_iterator_distance(last, end, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    // Write on the first iterator the contents of the last iterator, advance, and so on
    Iterator write_iterator = first;
    Iterator read_iterator = last;
    for (size_t i = 0; i < distance; i++)
    {
        if (vector->free_callback(types_iterator_get(write_iterator)) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
        memcpy(types_iterator_get(write_iterator), types_iterator_get(read_iterator), vector->element_size);
        memset(types_iterator_get(read_iterator), '\0', vector->element_size);
        read_iterator = types_iterator_increase(read_iterator, 1);
        write_iterator = types_iterator_increase(write_iterator, 1);
    }

    // Reduce the size of the vector accordingly
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }
    vector->size -= distance;
    return CODE_OK;
}

ResultCode types_vector_insert(Vector *vector, Iterator first, Iterator last, Iterator destination)
{
    if (vector == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Compute the number of new elements
    size_t num_elements_added = 0;
    if (types_iterator_distance(first, last, &num_elements_added) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    // Compute the number of elements that will be displaced
    size_t numElementsDisplaced = 0;
    if (types_iterator_distance(destination, types_vector_end(vector), &numElementsDisplaced) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }

    // Reallocate the vector to accomodate the new elements
    void *tmp = realloc(vector->data, (vector->capacity + num_elements_added) * vector->element_size);
    if (tmp == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    vector->data = tmp;
    vector->size += num_elements_added;
    vector->capacity += num_elements_added;

    // Move all the elements between destination and end, to the end of the vector
    // The last element is moved first to prevent overwrites. Then the iterator progresses down
    Iterator read_iterator, write_iterator;
    read_iterator = types_iterator_increase(destination, numElementsDisplaced - 1);
    write_iterator = types_iterator_increase(read_iterator, num_elements_added);
    for (size_t i = 0; i < numElementsDisplaced; i++)
    {
        memcpy(types_iterator_get(write_iterator), types_iterator_get(read_iterator), vector->element_size);
        read_iterator = types_iterator_decrease(read_iterator, 1);
        write_iterator = types_iterator_decrease(write_iterator, 1);
    }

    // Write the new elements
    read_iterator = first;
    write_iterator = destination;
    for (size_t i = 0; i < num_elements_added; i++)
    {
        memcpy(types_iterator_get(write_iterator), types_iterator_get(read_iterator), vector->element_size);
        read_iterator = types_iterator_increase(read_iterator, 1);
        write_iterator = types_iterator_increase(write_iterator, 1);
    }

    return CODE_OK;
}

bool types_vector_empty(const Vector *vector)
{
    if (vector == NULL)
    {
        return true;
    }
    return types_vector_size(vector) == 0;
}