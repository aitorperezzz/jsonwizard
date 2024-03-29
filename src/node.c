#include <stdio.h>
#include <string.h>

#include "node.h"
#include "types/types_vector.h"
#include "types/types_map.h"
#include "write.h"
#include "read/read.h"

Node *node_create()
{
    Node *node = malloc(sizeof(Node));
    if (node == NULL)
    {
        return NULL;
    }

    // Default values
    node->type = NODE_TYPE_NULL;
    node->parent = NULL;
    node->data = NULL;
    return node;
}

String *node_to_string(const Node *node)
{
    return write_to_string(node);
}

Node *node_from_string(const String *string)
{
    return read_from_string(string);
}

NodeType node_get_type(const Node *node)
{
    if (node == NULL)
    {
        return NODE_TYPE_NULL;
    }
    return node->type;
}

Node *node_get_parent(const Node *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->parent;
}

Node *node_get(Node *node, const String *key)
{
    if (node == NULL || key == NULL)
    {
        return NULL;
    }

    // The node has to be of type object in order to look
    // for a key
    if (node->type != NODE_TYPE_OBJECT)
    {
        return NULL;
    }
    if (node->data == NULL)
    {
        return NULL;
    }

    // If the node is of type object, the data is a map
    Map *map = node->data;
    return (Node *)types_map_at(map, key);
}

ResultCode node_append(Node *node, const String *key, const Node *child)
{
    if (node == NULL || key == NULL || child == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // This function can only be used if the node is of type object
    if (node->type != NODE_TYPE_OBJECT)
    {
        return CODE_LOGIC_ERROR;
    }

    // Add the key value to the existant map
    Map *map = node->data;
    if (map == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    types_map_insert(map, key, child);

    return CODE_OK;
}

ResultCode node_erase(Node *node)
{
    if (node == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // If this node has no parent, I cannot continue
    if (node->parent == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    switch (node->parent->type)
    {
    case NODE_TYPE_NULL:
    case NODE_TYPE_TRUE:
    case NODE_TYPE_FALSE:
    case NODE_TYPE_NUMBER:
    case NODE_TYPE_STRING:
        return CODE_MEMORY_ERROR;
    case NODE_TYPE_ARRAY:
    {
        Vector *vector = node->parent->data;
        for (Iterator current = types_vector_begin(vector), end = types_vector_end(vector);
             !types_iterator_equal(current, end);
             types_iterator_increase(current, 1))
        {
            if (types_iterator_get(current) == node)
            {
                // This is the one that needs to be removed
                return types_vector_erase(vector, current, types_iterator_increase(current, 1));
            }
        }
        return CODE_LOGIC_ERROR;
    }
    case NODE_TYPE_OBJECT:
    {
        Map *map = node->parent->data;
        for (Iterator current = types_map_begin(map), end = types_map_end(map);
             !types_iterator_equal(current, end);
             types_iterator_increase(current, 1))
        {
            Pair *pair = types_iterator_get(current);
            if (pair->value == node)
            {
                // This is the one that needs to be removed
                return types_map_erase(map, current, types_iterator_increase(current, 1));
            }
        }
        return CODE_LOGIC_ERROR;
    }
    }
}

ResultCode node_set_key(Node *node, const String *key)
{
    if (node == NULL || key == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // If this node does not have a parent, I cannot continue
    if (node->parent == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // If the parent is not of type object, I cannot continue
    if (node->parent->type != NODE_TYPE_OBJECT)
    {
        return CODE_LOGIC_ERROR;
    }

    // Find the key value pair inside the parent
    Map *map = node->parent->data;
    for (Iterator current = types_map_begin(map), end = types_map_end(map);
         !types_iterator_equal(current, end);
         types_iterator_increase(current, 1))
    {
        Pair *pair = types_iterator_get(current);
        if (pair->value == node)
        {
            // Change the key here because we know the index
            types_string_free(pair->key);
            free(pair->key);
            pair->key = types_string_copy(key);
            return CODE_OK;
        }
    }

    return CODE_LOGIC_ERROR;
}

ResultCode node_set_data(Node *node, const Node *new)
{
    if (node == NULL || new == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Clean the memory used by the previous data
    node_free(node);

    // Assign new data
    node->type = new->type;
    node->data = new->data;
    return CODE_OK;
}

ResultCode node_array_push(Node *root, Node *node)
{
    if (root == NULL || node == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Root node needs to be of type array
    if (root->type != NODE_TYPE_ARRAY)
    {
        return CODE_LOGIC_ERROR;
    }

    // Push it to the existing list
    Vector *vector = root->data;
    if (types_vector_push(vector, node) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }
    return CODE_OK;
}

Iterator node_array_begin(Node *node)
{
    if (node == NULL)
    {
        return types_iterator_invalid();
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return types_iterator_invalid();
    }

    Vector *vector = node->data;
    return types_vector_begin(vector);
}

Iterator node_array_end(Node *node)
{
    if (node == NULL)
    {
        return types_iterator_invalid();
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return types_iterator_invalid();
    }

    Vector *vector = node->data;
    return types_vector_end(vector);
}

ResultCode node_array_insert(Node *node, Iterator first, Iterator last, Iterator destination)
{
    if (node == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return CODE_LOGIC_ERROR;
    }

    Vector *vector = node->data;
    return types_vector_insert(vector, first, last, destination);
}

size_t node_array_size(Node *node)
{
    if (node == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return CODE_LOGIC_ERROR;
    }

    return types_vector_size(node->data);
}

Node *node_array_get(Node *node, size_t index)
{
    if (node == NULL)
    {
        return NULL;
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return NULL;
    }

    Vector *vector = node->data;
    return (Node *)types_vector_at(vector, index);
}

ResultCode node_free(void *node)
{
    ResultCode result = CODE_OK;
    Node *my_node = node;

    // Free resources according to the type
    switch (my_node->type)
    {
    case NODE_TYPE_NULL:
    case NODE_TYPE_NUMBER:
    case NODE_TYPE_TRUE:
    case NODE_TYPE_FALSE:
        break;
    case NODE_TYPE_STRING:
        result = types_string_free(my_node->data);
        break;
    case NODE_TYPE_ARRAY:
        result = types_vector_free(my_node->data);
        break;
    case NODE_TYPE_OBJECT:
        result = types_map_free(my_node->data);
        break;
    }

    if (my_node->data != NULL)
    {
        free(my_node->data);
        my_node->data = NULL;
    }
    my_node->type = NODE_TYPE_NULL;

    return result;
}