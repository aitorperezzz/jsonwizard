#ifndef NODE_H
#define NODE_H

#include "string_type.h"

// Types of nodes
typedef enum
{
    NODE_TYPE_NULL, // Node does not have a valid type
    NODE_TYPE_STRING,
    NODE_TYPE_INTEGER,
    NODE_TYPE_BOOLEAN,
    NODE_TYPE_ARRAY,
    NODE_TYPE_OBJECT
} NodeType;

// Types of fields inside a node
typedef enum
{
    FIELD_TYPE,
    FIELD_KEY,
    FIELD_PARENT,
    FIELD_DATA
} NodeField;

// Possible boolean values
typedef enum
{
    BOOL_TRUE,
    BOOL_FALSE,
    BOOL_UNKNOWN
} Boolean;

// Variables inside a node
typedef struct Node_st
{
    NodeType type;
    String *key;
    struct Node_st *parent;
    void *data;
} Node;

#endif