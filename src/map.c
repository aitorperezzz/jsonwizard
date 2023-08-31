#include <string.h>
#include <stdio.h>

#include "map.h"
#include "vector.h"

static ResultCode map_freePair(void *pairRaw)
{
    if (!pairRaw)
    {
        return CODE_OK;
    }
    Pair *pair = pairRaw;
    // Call the free callbacks if they are available
    if (pair->closure.keyFreeCallback != NULL)
    {
        if (pair->closure.keyFreeCallback(pair->key) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }
    if (pair->closure.valueFreeCallback != NULL)
    {
        if (pair->closure.valueFreeCallback(pair->value) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }
    return CODE_OK;
}

Map *map_create(const size_t keySize, const size_t valueSize,
                ResultCode (*keyFreeCallback)(void *key1),
                ResultCode (*valueFreeCallback)(void *key1),
                bool (*keyCompareCallback)(const void *, const void *),
                void *(*keyCopyCallback)(const void *),
                void *(*valueCopyCallback)(const void *))
{
    Map *map = malloc(sizeof(Map));
    if (map == NULL)
    {
        return NULL;
    }

    // Simple elements of the map
    map->keySize = keySize;
    map->valueSize = valueSize;
    map->keyCompareCallback = keyCompareCallback;
    map->keyCopyCallback = keyCopyCallback;
    map->valueCopyCallback = valueCopyCallback;
    // Create the closure of the map, that stores the two callbacks needed to free
    // the key and the value respectively
    map->closure.keyFreeCallback = keyFreeCallback;
    map->closure.valueFreeCallback = valueFreeCallback;

    // Create the elements as a vector of pairs
    map->elements = vector_create(sizeof(Pair), map_freePair);
    if (map->elements == NULL)
    {
        return NULL;
    }
    return map;
}

void *map_at(Map *map, const void *key)
{
    if (map == NULL || key == NULL)
    {
        return NULL;
    }

    // Find the element
    Iterator pairIterator = map_find(map, key);
    if (iterator_equal(pairIterator, iterator_invalid()))
    {
        return NULL;
    }
    return ((Pair *)iterator_get(pairIterator))->value;
}

Iterator map_find(const Map *map, const void *key)
{
    if (map == NULL || key == NULL)
    {
        return iterator_invalid();
    }

    // Iterate over the values of the internal vector
    Pair *pair;
    Vector *elements = map->elements;
    for (size_t i = 0, n = vector_size(elements); i < n; i++)
    {
        // Access the current pair
        pair = vector_at(elements, i);
        // Check if this key is the same as the one provided
        if (map->keyCompareCallback(pair->key, key) == 0)
        {
            return iterator_create(pair, sizeof(pair));
        }
    }
    return iterator_invalid();
}

Iterator map_begin(const Map *map)
{
    if (map == NULL)
    {
        return iterator_invalid();
    }
    return vector_begin(map->elements);
}

Iterator map_end(const Map *map)
{
    if (map == NULL)
    {
        return iterator_invalid();
    }
    return vector_end(map->elements);
}

bool map_empty(const Map *map)
{
    if (map == NULL)
    {
        return true;
    }
    return vector_empty(map->elements);
}

size_t map_size(const Map *map)
{
    if (map == NULL)
    {
        return 0;
    }
    return vector_size(map->elements);
}

ResultCode map_clear(Map *map)
{
    if (map == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    vector_clear(map->elements);
    free(map->elements);
    map->elements = NULL;
    // The closure and the other pointers need to remain.

    return CODE_OK;
}

Iterator map_insert(Map *map, const void *key, const void *value)
{
    if (map == NULL || key == NULL || value == NULL)
    {
        return iterator_invalid();
    }

    // Create the new pair to push to the map
    Pair pair;
    pair.key = map->keyCopyCallback(key);
    pair.value = map->valueCopyCallback(value);
    // Copy the callbacks in the closure
    pair.closure.keyFreeCallback = map->closure.keyFreeCallback;
    pair.closure.valueFreeCallback = map->closure.valueFreeCallback;
    if (vector_push(map->elements, &pair) != CODE_OK)
    {
        return iterator_invalid();
    }

    // Return an iterator to the last element we have pushed
    Iterator iterator = vector_end(map->elements);
    return iterator_decrease(iterator, 1);
}

ResultCode map_erase(Map *map, Iterator first, Iterator last)
{
    if (map == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    return vector_erase(map->elements, first, last);
}

ResultCode map_free(Map *map)
{
    if (map == NULL)
    {
        return CODE_OK;
    }

    // Free the internal vector
    if (vector_free(map->elements) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    free(map->elements);
    map->elements = NULL;

    // The rest of the variables don't need to be freed
    return CODE_OK;
}