#include "json_path.h"

typedef union PathElementData_u
{
    int index;
    String *key;
} PathElementData;

enum PathElementType
{
    PATH_TYPE_STRING,
    PATH_TYPE_INDEX
};

typedef struct PathElement_st
{
    enum PathElementType type;
    PathElementData data;
} PathElement;

static JsonPath *json_path_createInvalid();

JsonPath *json_path_parse(const String *string)
{
    return json_path_createInvalid();
}

JsonPath *json_path_decrease(const JsonPath *original)
{
    return json_path_createInvalid();
}

String *json_path_leaf(const JsonPath *path)
{
    return NULL;
}

Node *json_path_get(const JsonPath *jsonPath, Node *node)
{
    return NULL;
}

static JsonPath *json_path_createInvalid()
{
    JsonPath *path = malloc(sizeof(JsonPath));
    if (!path)
    {
        return NULL;
    }
    path->original = NULL;
    path->path = NULL;
    return path;
}