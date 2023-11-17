#include <string.h>

#include "types_iterator.h"

/// @brief Check if the list of iterators is consistent, meaning the sizes pointed at
/// are all the same
/// @param list List of iterators to check
/// @param number Number of iterators to check
/// @return True if the iterators are consistent
static bool types_iterator_check_consistency(Iterator *const *list, const size_t number)
{
    if (list == NULL)
    {
        return false;
    }
    if (number == 0 || number == 1)
    {
        return true;
    }

    // Check if all the iterators provided are equal to the first one
    for (size_t index = 1; index < number; index++)
    {
        if (list[index]->size != list[0]->size)
        {
            return false;
        }
    }
    return true;
}

Iterator types_iterator_invalid(void)
{
    Iterator iterator;
    iterator.pointer = NULL;
    iterator.size = 0;
    return iterator;
}

Iterator types_iterator_create(void *pointer, const size_t size)
{
    Iterator iterator;
    iterator.pointer = pointer;
    iterator.size = size;
    return iterator;
}

Iterator types_iterator_copy(Iterator first, Iterator last, Iterator result)
{
    // Check consistency of iterators
    Iterator *iterator_list[3];
    iterator_list[0] = &first;
    iterator_list[1] = &last;
    iterator_list[2] = &result;
    if (!types_iterator_check_consistency(iterator_list, 3))
    {
        return types_iterator_invalid();
    }

    // Check if result points to some element between first and last, which is not allowed
    size_t distance1, distance2;
    if (types_iterator_distance(first, result, &distance1) != CODE_OK)
    {
        return types_iterator_invalid();
    }
    if (types_iterator_distance(first, last, &distance2) != CODE_OK)
    {
        return types_iterator_invalid();
    }
    if (distance1 < distance2)
    {
        return types_iterator_invalid();
    }

    // Finally perform the copy
    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return types_iterator_invalid();
    }
    for (size_t i = 0; i < distance; i++)
    {
        memcpy(result.pointer, first.pointer, first.size);
        first = types_iterator_increase(first, 1);
        result = types_iterator_increase(result, 1);
    }
    return result;
}

Iterator types_iterator_find(Iterator first, Iterator last, const void *data)
{
    // Check consistency of iterators
    Iterator *iterator_list[2];
    iterator_list[0] = &first;
    iterator_list[1] = &last;
    if (!types_iterator_check_consistency(iterator_list, 2))
    {
        return types_iterator_invalid();
    }

    // Check invalid input parameter
    if (!data)
    {
        return types_iterator_invalid();
    }

    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return types_iterator_invalid();
    }
    for (size_t i = 0; i < distance; i++)
    {
        if (memcmp(first.pointer, data, first.size) == 0)
        {
            return first;
        }
        first = types_iterator_increase(first, 1);
    }
    return first;
}

bool types_iterator_equal(Iterator iterator1, Iterator iterator2)
{
    return iterator1.pointer == iterator2.pointer && iterator1.size == iterator2.size;
}

void *types_iterator_get(Iterator iterator)
{
    return iterator.pointer;
}

ResultCode types_iterator_distance(Iterator first, Iterator last, size_t *distance)
{
    // Check consistency of iterators
    Iterator *iterator_list[2];
    iterator_list[0] = &first;
    iterator_list[1] = &last;
    if (!types_iterator_check_consistency(iterator_list, 2))
    {
        return CODE_LOGIC_ERROR;
    }

    // Check output parameter has been provided
    if (!distance)
    {
        return CODE_MEMORY_ERROR;
    }

    *distance = (last.pointer - first.pointer) / first.size;
    return CODE_OK;
}

Iterator types_iterator_increase(Iterator iterator, const size_t value)
{
    Iterator result = iterator;
    result.pointer += iterator.size * value;
    return result;
}

Iterator types_iterator_decrease(Iterator iterator, const size_t value)
{
    Iterator result = iterator;
    result.pointer -= iterator.size * value;
    return result;
}