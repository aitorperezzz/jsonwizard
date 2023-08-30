#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "node.h"
#include "vector.h"

typedef struct JsonPath_st
{
    String *original;
    Vector *path;
} JsonPath;

JsonPath *json_path_parse(const String *string);

JsonPath *json_path_decrease(const JsonPath *original);

String *json_path_leaf(const JsonPath *path);

Node *json_path_get(const JsonPath *jsonPath, Node *node);

#endif