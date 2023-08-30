#include <stdio.h>
#include <string.h>

#include "node.h"
#include "vector.h"
#include "parse.h"
#include "map.h"

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
    // TODO
    return NULL;
}

Node *node_from_string(const String *string)
{
    // TODO
    return NULL;
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
    return (Node *)map_at(map, key);
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
    map_insert(node->data, key, child);

    return CODE_OK;
}

ResultCode node_erase(Node *node, const String *key)
{
    if (node == NULL || key == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // This function can only be used if the node is of type object
    if (node->type != NODE_TYPE_OBJECT)
    {
        return CODE_LOGIC_ERROR;
    }

    // Check if the key can be found inside this node
    Map *map = node->data;
    Iterator iterator = map_find(map, key);
    Iterator end = map_end(map);
    if (iterator_equal(iterator, end))
    {
        return CODE_LOGIC_ERROR;
    }

    // Remove the element at the iterator just found
    Iterator last = iterator_increase(iterator, 1);
    if (map_erase(map, iterator, last) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    return CODE_OK;
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
    for (size_t i = 0, n = map_size(map); i < n; i++)
    {
        MapElement *element = vector_at(map->elements, i);
        if (element->value == node)
        {
            // Change the key here because we know the index
            element->key = string_copy(key);
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

    // TODO:
    // the data needs to be parsed to check the type
    // clean the previous value of the complete node
    // assign the new type of the node
    // assign the new data of the node

    return CODE_NOT_SUPPORTED;
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
    if (vector_push(vector, node) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }
    return CODE_OK;
}

Iterator node_array_begin(Node *node)
{
    if (node == NULL)
    {
        return iterator_invalidIterator();
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return iterator_invalidIterator();
    }

    Vector *vector = node->data;
    return vector_begin(vector);
}

Iterator node_array_end(Node *node)
{
    if (node == NULL)
    {
        return iterator_invalidIterator();
    }
    if (node->type != NODE_TYPE_ARRAY)
    {
        return iterator_invalidIterator();
    }

    Vector *vector = node->data;
    return vector_end(vector);
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
    return vector_insert(vector, first, last, destination);
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

    return vector_size(node->data);
}

ResultCode node_free(Node *node)
{
    ResultCode result = CODE_OK;
    switch (node->type)
    {
    case NODE_TYPE_NULL:
    case NODE_TYPE_NUMBER:
    case NODE_TYPE_BOOLEAN:
        break;
    case NODE_TYPE_STRING:
        result = string_free(node->data);
        break;
    case NODE_TYPE_ARRAY:
        result = vector_free(node->data);
        break;
    case NODE_TYPE_OBJECT:
        result = map_free(node->data);
        break;
    }

    if (node->data != NULL)
    {
        free(node->data);
        node->data = NULL;
    }
    node->type = NODE_TYPE_NULL;

    return result;
}

ResultCode node_free_raw(void *rawNode)
{
    return node_free((Node *)rawNode);
}