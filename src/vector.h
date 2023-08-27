#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#include "utils.h"
#include "iterator.h"

/// @brief Definition of the contents of a vector
typedef struct Vector_st
{
    void *data;
    size_t size;
    size_t capacity;
    size_t elementSize;
    ResultCode (*freeCallback)(void *);
} Vector;

/// @brief Creates a new vector
/// @param elementSize Size of each of the elements it will store
/// @param freeCallback Callback function to free the memory used at specific location in the vector
/// @return The newly created vector, or NULL if something went wrong
Vector *vector_create(const size_t elementSize, ResultCode (*freeCallback)(void *));

/// @brief Returns the number of elements currently in the vector
/// @param vector The vector
/// @return The number of elements currently in the vector
size_t vector_size(const Vector *vector);

/// @brief Adds an element to the end of a vector
/// @param vector The vector
/// @param data Pointer to the data to add
/// @return Result code
ResultCode vector_push(Vector *vector, const void *data);

/// @brief Clears a vector: all the elements are removed and the size goes to 0. However, the capacity
/// does not change, so the memory reserved stays there
/// @param vector The vector to clear
/// @return Result code
ResultCode vector_clear(Vector *vector);

/// @brief Frees a vector structure. It frees the memory used by the internal elements of the vector, then
/// frees the memory used by the structure itself
/// @param vector The vector to free
/// @return Result code
ResultCode vector_free(Vector *vector);

/// @brief Returns a pointer to the data inside the vector at the specified index
/// @param vector The vector
/// @param index Index of the element to return
/// @return Pointer to the data at the provided index
void *vector_at(const Vector *vector, const size_t index);

/// @brief Changes the data inside the vector at the specified index
/// @param vector Vector to modify
/// @param index Index where the data will be stored
/// @param data Data to store
/// @return Result code
ResultCode vector_set(Vector *vector, const size_t index, const void *data);

/// @brief Returns an iterator pointing to the beginning of the vector
/// @param vector Vector
/// @return Iterator at the beginning of the vector
Iterator vector_begin(const Vector *vector);

/// @brief Returns an iterator pointing to the end of the vector
/// @param vector Vector
/// @return Iterator at the end of the vector. Caution, most probably, this iterator points to inaccessible memory
Iterator vector_end(const Vector *vector);

/// @brief Erases a range of elements from the vector
/// @param vector The vector where the elements will be erased
/// @param first Iterator to the first element to remove
/// @param last Iterator to the last (past) element to remove
/// @return Result code
ResultCode vector_erase(Vector *vector, Iterator first, Iterator last);

ResultCode vector_insert(Vector *vector, Iterator first, Iterator last, Iterator destination);

bool vector_empty(const Vector *vector);

#endif