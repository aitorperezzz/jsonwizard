#ifndef NODE_H
#define NODE_H

#include "string_type.h"
#include "iterator.h"

/// @brief Types of nodes
typedef enum
{
    NODE_TYPE_NULL,
    NODE_TYPE_STRING,
    NODE_TYPE_NUMBER,
    NODE_TYPE_BOOLEAN,
    NODE_TYPE_ARRAY,
    NODE_TYPE_OBJECT
} NodeType;

/// @brief Definition of a node structure
typedef struct Node_st
{
    NodeType type;
    struct Node_st *parent;
    void *data;
} Node;

Node *node_create();

String *node_to_string(const Node *node);

Node *node_from_string(const String *string);

NodeType node_get_type(const Node *node);

Node *node_get_parent(const Node *node);

Node *node_get(Node *node, const String *key);

ResultCode node_append(Node *node, const String *key, const Node *child);

ResultCode node_erase(Node *node);

ResultCode node_set_key(Node *node, const String *key);

ResultCode node_set_data(Node *node, const Node *new);

ResultCode node_array_push(Node *root, Node *node);

Iterator node_array_begin(Node *node);

Iterator node_array_end(Node *node);

ResultCode node_array_insert(Node *node, Iterator first, Iterator last, Iterator destination);

size_t node_array_size(Node *node);

Node *node_array_get(Node *node, size_t index);

ResultCode node_free(Node *node);

ResultCode node_free_raw(void *rawNode);

#endif