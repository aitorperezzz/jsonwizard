#ifndef TYPES_VECTOR_H
#define TYPES_VECTOR_H

#include <stdlib.h>

#include "utils.h"
#include "types/types_iterator.h"

/// @brief Vector
typedef struct Vector_st
{
    void *data;                          // Internal back-to-back buffer of data
    size_t size;                         // Number of elements currently in the vector
    size_t capacity;                     // Total number of elements that can fit in the reserved internal buffer
    size_t element_size;                 // Number of bytes of each item in the vector
    ResultCode (*free_callback)(void *); // Function to call to free the memory used by one element of the vector
} Vector;

/// @brief Create a new vector
/// @param element_size Number of bytes of each element that will be stored in the vector
/// @param free_callback Function to call to free the memory used by one element of the vector
/// @retval Pointer to the new vector
/// @retval NULL if a problem was encountered
Vector *types_vector_create(const size_t element_size, ResultCode (*free_callback)(void *));

/// @brief Return the number of elements of the vector
/// @param vector The vector
/// @return Number of elements of the vector
size_t types_vector_size(const Vector *vector);

/// @brief Push an element to the end of the vector
/// @param vector The vector
/// @param data Pointer to the data to add
/// @return Result code
ResultCode types_vector_push(Vector *vector, const void *data);

/// @brief Remove all the elements in the vector
/// @param vector The vector that will be cleared
/// @return Result code
ResultCode types_vector_clear(Vector *vector);

/// @brief Free all the memory used by the vector, including each of the internal elements
/// @param vector The vector that will be freed
/// @return Result code
ResultCode types_vector_free(Vector *vector);

/// @brief Return a pointer to the data inside the vector at the specified index
/// @param vector The vector
/// @param index Index of the element being accessed
/// @retval Pointer to the data at the provided index
/// @retval NULL if the vector or the index are not valid
void *types_vector_at(const Vector *vector, const size_t index);

/// @brief Change the data inside the vector at the specified index
/// @param vector Vector to modify
/// @param index Index where the data will be stored
/// @param data Data to store
/// @return Result code
ResultCode types_vector_set(Vector *vector, const size_t index, const void *data);

/// @brief Return an iterator pointing at the beginning of the vector
/// @param vector Vector
/// @return Iterator pointing at the beginning of the vector
Iterator types_vector_begin(const Vector *vector);

/// @brief Return an iterator pointing at the end of the vector
/// @param vector Vector
/// @return Iterator pointing at the end (past) of the vector
Iterator types_vector_end(const Vector *vector);

/// @brief Erase a range of elements from the vector
/// @param vector The vector where the elements will be erased
/// @param first Iterator to the first element to remove
/// @param last Iterator to the last (past) element to remove
/// @return Result code
ResultCode types_vector_erase(Vector *vector, Iterator first, Iterator last);

/// @brief Insert a whole range of elements into the vector
/// @param vector Vector that will be modified
/// @param first Iterator pointing to the first element of the range that will be inserted
/// @param last Iterator pointing to the last (past) element of the range that will be inserted
/// @param destination Iterator pointing to the first element of the destination range (inside the vector)
/// @return Result code
ResultCode types_vector_insert(Vector *vector, Iterator first, Iterator last, Iterator destination);

/// @brief Return true if the vector is empty
/// @param vector The vector
/// @return true if the vector is empty, false if not
bool types_vector_empty(const Vector *vector);

#endif