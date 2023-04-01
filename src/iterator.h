#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>
#include <stdbool.h>

/// @brief Definition of the contents of an iterator
typedef struct Iterator_st
{
    void *pointer;
    size_t size;
} Iterator;

/// @brief Creates an iterator
/// @param pointer Internal pointer that will be held by the iterator
/// @param size Size of the elements, in bytes, pointed at by the returned iterator
/// @return Iterator
Iterator iterator_create(void *pointer, const size_t size);

/// @brief Copies from first to last, into the iterator provided in result
/// The result iterator shall not point to an element between first and last
/// @param first Iterator that marks the beginning of the section to copy
/// @param last Iterator that marks the end (past) of the section to copy
/// @param result Iterator that marks where the data will start to be copied
/// @return Iterator to the end (past) of the region that has been copied
Iterator iterator_copy(Iterator first, Iterator last, Iterator result);

/// @brief Finds if an element exists within the range of iterators provided
/// @param first Iterator that marks the start of the section to search
/// @param last Iterator that marks the end (past) of the section to search
/// @param data Pointer to the data that needs to be found. The function will check
/// as many bytes as the internal size of the iterators
/// @return Iterator pointing to the element found, or last if the element was not found
Iterator iterator_find(Iterator first, Iterator last, const void *data);

/// @brief Decides if two iterators are the same
/// @param iterator1 Iterator 1
/// @param iterator2 Iterator 2
/// @return Boolean signifying if the iterators are the same or not
bool iterator_equal(Iterator iterator1, Iterator iterator2);

/// @brief Returns the pointer to the data pointed at by the iterator
/// @param iterator Iterator
/// @return Data pointed at by the iterator
void *iterator_get(Iterator iterator);

/// @brief Computes the distance, in number of elements, not bytes, between the iterators
/// @param first First iterator
/// @param last Last iterator
/// @return Number of elements between the two iterators
size_t iterator_distance(Iterator first, Iterator last);

/// @brief Increases the internal pointer of the iterator by the number of elements provided
/// @param iterator Iterator that will be advanced
/// @param value Number of elements to advance the iterator
/// @return Iterator advanced the number of elements requested
Iterator iterator_increase(Iterator iterator, const size_t value);

/// @brief Decreases the internal pointer of the iterator by the number of elements provided
/// @param iterator Iterator that will be decreased
/// @param value Number of elements to decrease the iterator
/// @return Iterator decreased the number of elements requested
Iterator iterator_decrease(Iterator iterator, const size_t value);

#endif