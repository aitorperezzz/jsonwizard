#include <string.h>
#include <stdio.h>

#include "iterator.h"

Iterator iterator_invalid(void)
{
    Iterator iterator;
    iterator.pointer = NULL;
    iterator.size = 0;
    return iterator;
}

Iterator iterator_create(void *pointer, const size_t size)
{
    Iterator iterator;
    iterator.pointer = pointer;
    iterator.size = size;
    return iterator;
}

Iterator iterator_copy(Iterator first, Iterator last, Iterator result)
{
    // Check if result points to some element between first and last, which is not allowed
    if (iterator_distance(first, result) < iterator_distance(first, last))
    {
        printf("Cannot copy to a range between first and last\n");
        return iterator_invalid();
    }
    const size_t distance = iterator_distance(first, last);
    for (size_t i = 0; i < distance; i++)
    {
        memcpy(result.pointer, first.pointer, first.size);
        first = iterator_increase(first, 1);
        result = iterator_increase(result, 1);
    }
    return result;
}

Iterator iterator_find(Iterator first, Iterator last, const void *data)
{
    const size_t distance = iterator_distance(first, last);
    for (size_t i = 0; i < distance; i++)
    {
        if (memcmp(first.pointer, data, first.size) == 0)
        {
            return first;
        }
        first = iterator_increase(first, 1);
    }
    return first;
}

bool iterator_equal(Iterator iterator1, Iterator iterator2)
{
    return iterator1.pointer == iterator2.pointer && iterator1.size == iterator2.size;
}

void *iterator_get(Iterator iterator)
{
    return iterator.pointer;
}

size_t iterator_distance(Iterator first, Iterator last)
{
    return (last.pointer - first.pointer) / first.size;
}

Iterator iterator_increase(Iterator iterator, const size_t value)
{
    Iterator result = iterator;
    result.pointer += iterator.size * value;
    return result;
}

Iterator iterator_decrease(Iterator iterator, const size_t value)
{
    Iterator result = iterator;
    result.pointer -= iterator.size * value;
    return result;
}