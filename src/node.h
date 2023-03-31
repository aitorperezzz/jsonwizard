#ifndef NODE_H
#define NODE_H

#include "string_type.h"

/// @brief Types of nodes
typedef enum
{
    NODE_TYPE_NULL, // Node does not have a valid type
    NODE_TYPE_STRING,
    NODE_TYPE_NUMBER,
    NODE_TYPE_BOOLEAN,
    NODE_TYPE_ARRAY,
    NODE_TYPE_OBJECT
} NodeType;

/// @brief Modifyable fields on each node
typedef enum
{
    FIELD_TYPE,
    FIELD_KEY,
    FIELD_PARENT,
    FIELD_DATA
} NodeField;

/// @brief Definition of a node structure
typedef struct Node_st
{
    NodeType type;
    String *key;
    struct Node_st *parent;
    void *data;
} Node;

Node *nodeCreate(const String *name);

ResultCode nodeAppend(Node *node, const String *parent, const String *name);

Node *nodeGet(Node *root, const String *key);

ResultCode nodeSetType(Node *node, const NodeType type);

ResultCode nodeSetKey(Node *node, const String *key);

ResultCode nodeSetData(Node *node, const String *data);

ResultCode nodeErase(Node *root, const String *key);

#endif