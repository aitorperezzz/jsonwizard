#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>
#include <stdbool.h>

#include "utils.h"

/// @brief Definition of an iterator
typedef struct Iterator_st
{
    void *pointer; // Pointer to the internal data
    size_t size;   // Number of bytes to move when the iterator is increased or decreased
} Iterator;

/// @brief Create an iterator
/// @param pointer Internal pointer that will be held by the iterator
/// @param size Size of the elements, in bytes, pointed at by the returned iterator
/// @return Iterator
Iterator iterator_create(void *pointer, const size_t size);

/// @brief Return an invalid iterator, where the pointer is NULL and size is zero
/// @return Invalid iterator
Iterator iterator_invalid(void);

/// @brief Copy the range [first, last), into the iterator provided in result.
/// The result iterator shall not point to an element between first and last
/// @param first Iterator that marks the beginning of the section to copy
/// @param last Iterator that marks the end (past) of the section to copy
/// @param result Iterator that marks where the data will start to be copied
/// @retval Iterator to the end (past) of the region that has been copied
/// @retval The invalid iterator if there was a problem
Iterator iterator_copy(Iterator first, Iterator last, Iterator result);

/// @brief Find the first element in the range [first, last) that equals the provided data
/// @param first Iterator that marks the start of the section to search
/// @param last Iterator that marks the end (past) of the section to search
/// @param data Pointer to the data that needs to be found. This function will check
/// as many bytes as the internal size of the iterators
/// @retval Iterator pointing to the first element found if it exists
/// @retval last if the element was not found
/// @retval The invalid iterator if there was a problem
Iterator iterator_find(Iterator first, Iterator last, const void *data);

/// @brief Decide if two iterators are equal
/// @param iterator1 Iterator 1
/// @param iterator2 Iterator 2
/// @return True if the iterators are the same, false if not
bool iterator_equal(Iterator iterator1, Iterator iterator2);

/// @brief Get the data pointed at by the iterator
/// @param iterator Iterator
/// @return Internal pointer of the iterator
void *iterator_get(Iterator iterator);

/// @brief Compute the number of elements between two iterators
/// @param first First iterator
/// @param last Last iterator
/// @param distance Number of elements in between the iterators (not number of bytes)
/// @retval Result code
ResultCode iterator_distance(Iterator first, Iterator last, size_t *distance);

/// @brief Increase the internal pointer of the iterator by the number of elements provided
/// @param iterator Iterator
/// @param value Number of elements to increase the iterator
/// @return Iterator increased the number of elements requested
Iterator iterator_increase(Iterator iterator, const size_t value);

/// @brief Decrease the internal pointer of the iterator by the number of elements provided
/// @param iterator Iterator
/// @param value Number of elements to decrease the iterator
/// @return Iterator decreased the number of elements requested
Iterator iterator_decrease(Iterator iterator, const size_t value);

#endif