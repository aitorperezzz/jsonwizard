#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "vector.h"
#include "string_type.h"

enum PathStepId_e
{
    PATH_STEP_INDEX,
    PATH_STEP_KEY
};

union PathStepData_u
{
    int index;
    String key;
};

typedef struct PathStep_st
{
    enum PathStepId_e id;
    union PathStepData_u data;
} PathStep;

typedef struct JsonPath_st
{
    String *original; // A copy of the original json path
    bool valid;       // Indicates if the path has a valid syntax
    Vector *paths;    // Each path here is a vector of steps
} JsonPath;

JsonPath *json_path_parse(const String *string);

#endif