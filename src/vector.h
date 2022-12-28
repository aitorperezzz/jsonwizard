#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#include "utils.h"

typedef enum
{
    VECTOR_TYPE_STRING,
    VECTOR_TYPE_NODE
} VectorType;

typedef struct
{
    VectorType type;
    void *data;
    size_t size;
    size_t capacity;
    size_t elementSize;
} Vector;

Vector *vectorCreate(const VectorType type);
const size_t vectorSize(const Vector *vector);
ResultCode vectorAdd(Vector *vector, const void *data, const size_t dataSize);
ResultCode vectorClear(Vector *vector);
ResultCode vectorFree(Vector *vector);
void *vectorGet(const Vector *vector, const size_t index);
void vectorSet(Vector *vector, const size_t index, const void *data);
void vectorErase(Vector *vector, const size_t index);

#endif