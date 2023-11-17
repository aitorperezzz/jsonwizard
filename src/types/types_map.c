#include <string.h>
#include <stdio.h>

#include "types_map.h"
#include "types_vector.h"

static ResultCode types_map_free_pair(void *pair_raw)
{
    if (!pair_raw)
    {
        return CODE_OK;
    }
    Pair *pair = pair_raw;
    // Call the free callbacks if they are available
    if (pair->closure.key_free_callback != NULL)
    {
        if (pair->closure.key_free_callback(pair->key) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }
    if (pair->closure.value_free_callback != NULL)
    {
        if (pair->closure.value_free_callback(pair->value) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }
    return CODE_OK;
}

Map *types_map_create(const size_t key_size, const size_t value_size,
                      ResultCode (*key_free_callback)(void *key1),
                      ResultCode (*value_free_callback)(void *key1),
                      bool (*key_compare_callback)(const void *, const void *),
                      void *(*key_copy_callback)(const void *),
                      void *(*value_copy_callback)(const void *))
{
    Map *map = malloc(sizeof(Map));
    if (map == NULL)
    {
        return NULL;
    }

    // Simple elements of the map
    map->key_size = key_size;
    map->value_size = value_size;
    map->key_compare_callback = key_compare_callback;
    map->key_copy_callback = key_copy_callback;
    map->value_copy_callback = value_copy_callback;
    // Create the closure of the map, that stores the two callbacks needed to free
    // the key and the value respectively
    map->closure.key_free_callback = key_free_callback;
    map->closure.value_free_callback = value_free_callback;

    // Create the elements as a vector of pairs
    map->elements = types_vector_create(sizeof(Pair), types_map_free_pair);
    if (map->elements == NULL)
    {
        return NULL;
    }
    return map;
}

void *types_map_at(Map *map, const void *key)
{
    if (map == NULL || key == NULL)
    {
        return NULL;
    }

    // Find the element
    Iterator pair_iterator = types_map_find(map, key);
    if (types_iterator_equal(pair_iterator, types_iterator_invalid()))
    {
        return NULL;
    }
    return ((Pair *)types_iterator_get(pair_iterator))->value;
}

Iterator types_map_find(const Map *map, const void *key)
{
    if (map == NULL || key == NULL)
    {
        return types_iterator_invalid();
    }

    // Iterate over the values of the internal vector
    Pair *pair;
    Vector *elements = map->elements;
    for (size_t i = 0, n = types_vector_size(elements); i < n; i++)
    {
        // Access the current pair
        pair = types_vector_at(elements, i);
        // Check if this key is the same as the one provided
        if (map->key_compare_callback(pair->key, key) == 0)
        {
            return types_iterator_create(pair, sizeof(pair));
        }
    }
    return types_iterator_invalid();
}

Iterator types_map_begin(const Map *map)
{
    if (map == NULL)
    {
        return types_iterator_invalid();
    }
    return types_vector_begin(map->elements);
}

Iterator types_map_end(const Map *map)
{
    if (map == NULL)
    {
        return types_iterator_invalid();
    }
    return types_vector_end(map->elements);
}

bool types_map_empty(const Map *map)
{
    if (map == NULL)
    {
        return true;
    }
    return types_vector_empty(map->elements);
}

size_t types_map_size(const Map *map)
{
    if (map == NULL)
    {
        return 0;
    }
    return types_vector_size(map->elements);
}

ResultCode types_map_clear(Map *map)
{
    if (map == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    types_vector_clear(map->elements);
    free(map->elements);
    map->elements = NULL;
    // The closure and the other pointers need to remain.

    return CODE_OK;
}

Iterator types_map_insert(Map *map, const void *key, const void *value)
{
    if (map == NULL || key == NULL || value == NULL)
    {
        return types_iterator_invalid();
    }

    // Create the new pair to push to the map
    Pair pair;
    pair.key = map->key_copy_callback(key);
    pair.value = map->value_copy_callback(value);
    // Copy the callbacks in the closure
    pair.closure.key_free_callback = map->closure.key_free_callback;
    pair.closure.value_free_callback = map->closure.value_free_callback;
    if (types_vector_push(map->elements, &pair) != CODE_OK)
    {
        return types_iterator_invalid();
    }

    // Return an iterator to the last element we have pushed
    Iterator iterator = types_vector_end(map->elements);
    return types_iterator_decrease(iterator, 1);
}

ResultCode types_map_erase(Map *map, Iterator first, Iterator last)
{
    if (map == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    return types_vector_erase(map->elements, first, last);
}

ResultCode types_map_free(Map *map)
{
    if (map == NULL)
    {
        return CODE_OK;
    }

    // Free the internal vector
    if (types_vector_free(map->elements) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    free(map->elements);
    map->elements = NULL;

    // The rest of the variables don't need to be freed
    return CODE_OK;
}