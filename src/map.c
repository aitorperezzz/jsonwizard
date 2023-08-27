#include <string.h>
#include <stdio.h>

#include "map.h"
#include "vector.h"

ResultCode map_element_free(void *mapElement)
{
    MapElement *structMapElement = mapElement;
    if (structMapElement->closure.keyFreeCallback == NULL || structMapElement->closure.valueFreeCallback == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    structMapElement->closure.keyFreeCallback(structMapElement->key);
    structMapElement->closure.valueFreeCallback(structMapElement->value);
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

    // Create the elements as a vector of map elements
    map->elements = vector_create(sizeof(MapElement), map_element_free);
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

    // Iterate over the values of the internal vector
    MapElement *element;
    for (size_t i = 0, n = vector_size(map->elements); i < n; i++)
    {
        element = vector_at(map->elements, i);
        // Check if this key is the one provided
        if (map->keyCompareCallback(element->key, key) == 0)
        {
            return element->value;
        }
    }

    // The provided key has not been found in the map
    return NULL;
}

Iterator map_find(const Map *map, const String *key)
{
    // TODO
    return iterator_invalidIterator();
}

Iterator map_begin(const Map *map)
{
    if (map == NULL)
    {
        return iterator_invalidIterator();
    }
    return vector_begin(map->elements);
}

Iterator map_end(const Map *map)
{
    if (map == NULL)
    {
        return iterator_invalidIterator();
    }
    return vector_end(map->elements);
}

bool map_empty(const Map *map)
{
    if (map == NULL)
    {
        return false;
    }
    return vector_empty(map->elements);
}

size_t map_size(const Map *map)
{
    if (map == NULL)
    {
        return false;
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
        return iterator_invalidIterator();
    }

    // Create the new element to push to the map
    MapElement element;
    element.key = map->keyCopyCallback(key);
    element.value = map->valueCopyCallback(value);
    // Copy the callbacks in the closure
    element.closure.keyFreeCallback = map->closure.keyFreeCallback;
    element.closure.valueFreeCallback = map->closure.valueFreeCallback;
    if (vector_push(map->elements, &element) != CODE_OK)
    {
        return iterator_invalidIterator();
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
        return CODE_MEMORY_ERROR;
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