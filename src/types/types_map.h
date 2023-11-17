#ifndef TYPES_MAP_H
#define TYPES_MAP_H

#include <stdlib.h>

#include "utils.h"
#include "types_iterator.h"
#include "types_vector.h"

struct FreeCallbacksClosure
{
    ResultCode (*key_free_callback)(void *);
    ResultCode (*value_free_callback)(void *);
};

typedef struct Pair_st
{
    void *key;
    void *value;
    struct FreeCallbacksClosure closure;
} Pair;

/// @brief Definition of the contents of a vector
typedef struct Map_st
{
    Vector *elements;
    size_t key_size;
    size_t value_size;
    struct FreeCallbacksClosure closure;
    bool (*key_compare_callback)(const void *, const void *);
    void *(*key_copy_callback)(const void *);
    void *(*value_copy_callback)(const void *);
} Map;

Map *types_map_create(const size_t key_size, const size_t value_size,
                      ResultCode (*key_free_callback)(void *key1),
                      ResultCode (*value_free_callback)(void *key1),
                      bool (*key_compare_callback)(const void *, const void *),
                      void *(*key_copy_callback)(const void *),
                      void *(*value_copy_callback)(const void *));

void *types_map_at(Map *map, const void *key);

Iterator types_map_find(const Map *map, const void *key);

Iterator types_map_begin(const Map *map);

Iterator types_map_end(const Map *map);

bool types_map_empty(const Map *map);

size_t types_map_size(const Map *map);

ResultCode types_map_clear(Map *map);

Iterator types_map_insert(Map *map, const void *key, const void *value);

ResultCode types_map_erase(Map *map, Iterator first, Iterator last);

ResultCode types_map_free(Map *map);

#endif