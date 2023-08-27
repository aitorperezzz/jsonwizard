#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

#include "utils.h"
#include "iterator.h"
#include "vector.h"
#include "string_type.h"

struct FreeFunctionsClosure
{
    ResultCode (*keyFreeCallback)(void *);
    ResultCode (*valueFreeCallback)(void *);
};

typedef struct MapElement_st
{
    void *key;
    void *value;
    struct FreeFunctionsClosure closure;
} MapElement;

/// @brief Definition of the contents of a vector
typedef struct Map_st
{
    Vector *elements;
    size_t keySize;
    size_t valueSize;
    struct FreeFunctionsClosure closure;
    bool (*keyCompareCallback)(const void *, const void *);
    void *(*keyCopyCallback)(const void *);
    void *(*valueCopyCallback)(const void *);
} Map;

Map *map_create(const size_t keySize, const size_t valueSize,
                ResultCode (*keyFreeCallback)(void *key1),
                ResultCode (*valueFreeCallback)(void *key1),
                bool (*keyCompareCallback)(const void *, const void *),
                void *(*keyCopyCallback)(const void *),
                void *(*valueCopyCallback)(const void *));

void *map_at(Map *map, const void *key);

Iterator map_find(const Map *map, const String *key);

Iterator map_begin(const Map *map);

Iterator map_end(const Map *map);

bool map_empty(const Map *map);

size_t map_size(const Map *map);

ResultCode map_clear(Map *map);

Iterator map_insert(Map *map, const void *key, const void *value);

ResultCode map_erase(Map *map, Iterator first, Iterator last);

ResultCode map_free(Map *map);

#endif