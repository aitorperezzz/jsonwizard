#include <stdio.h>
#include <string.h>

#include "node.h"
#include "vector.h"
#include "parse.h"

static ResultCode nodeInitialise(Node *node);

Node *nodeCreate(const String *name)
{
    Node *result = malloc(sizeof(Node));
    if (result == NULL)
    {
        return NULL;
    }
    // type
    result->type = NODE_TYPE_NULL;
    // key
    result->key = string_create();
    if (result->key == NULL)
    {
        return NULL;
    }
    if (string_copy(result->key, name) != CODE_OK)
    {
        return NULL;
    }
    // parent
    result->parent = NULL;
    // data
    result->data = NULL;

    return result;
}

ResultCode nodeAppend(Node *node, const String *parent, const String *name)
{
    if (node == NULL || parent == NULL || name == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Look for the parent key inside the node
    Node *parentNode = nodeGet(node, parent);
    if (parentNode == NULL)
    {
        printf("Key %s does not exist in node with key %s", string_cStr(parent), string_cStr(node->key));
        return CODE_LOGIC_ERROR;
    }

    // Only if the parent is of type object
    if (parentNode->type != NODE_TYPE_OBJECT)
    {
        printf("Cannot append node to node of type %d", parentNode->type);
        return CODE_LOGIC_ERROR;
    }

    // Create the child node
    Node *child = nodeCreate(name);
    if (child == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Append the child node to the existing nodes
    Vector *objects = (Vector *)parentNode->data;
    if (objects == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (vector_push(objects, &child) != CODE_OK)
    {
        return CODE_ERROR;
    }

    return CODE_OK;
}

Node *nodeGet(Node *root, const String *key)
{
    if (root == NULL || key == NULL)
    {
        return NULL;
    }

    if (root->type != NODE_TYPE_OBJECT)
    {
        if (string_compare(root->key, key) == 0)
        {
            return root;
        }
        else
        {
            return NULL;
        }
    }

    // This is an object node, so iterate over all its nodes and look recursively
    Vector *objects = (Vector *)root->data;
    Node *current = NULL;
    for (size_t i = 0, n = vector_size(objects); i < n; i++)
    {
        current = nodeGet(vector_get(objects, i), key);
        if (current != NULL)
        {
            return current;
        }
    }
    return NULL;
}

ResultCode nodeSetType(Node *node, const NodeType type)
{
    // First reset the node
    if (nodeFree(node) != CODE_OK)
    {
        return CODE_ERROR;
    }

    // Set the type
    node->type = type;

    // Initialise according to the type
    if (nodeInitialise(node) != CODE_OK)
    {
        return CODE_ERROR;
    }

    return CODE_OK;
}

ResultCode nodeSetKey(Node *node, const String *key)
{
    if (node == NULL || key == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    string_copy(node->key, key);
    return CODE_OK;
}

ResultCode nodeSetData(Node *node, const String *data)
{
    if (node == NULL || data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    switch (node->type)
    {
    case NODE_TYPE_NULL:
        printf("Cannot set data on node of type null");
        return CODE_LOGIC_ERROR;
    case NODE_TYPE_STRING:
        string_copy(node->data, data);
        return CODE_OK;
    case NODE_TYPE_NUMBER:
    {
        double value;
        if (parseNumber(data, &value) != CODE_OK)
        {
            return CODE_FORMAT_ERROR;
        }
        memcpy(node->data, &value, sizeof(value));
        return CODE_OK;
    }

    case NODE_TYPE_BOOLEAN:
    {
        Boolean value;
        if (parseBoolean(data, &value) != CODE_OK)
        {
            return CODE_FORMAT_ERROR;
        }
        memcpy(node->data, &value, sizeof(value));
        return CODE_OK;
    }

    case NODE_TYPE_ARRAY:
    case NODE_TYPE_OBJECT:
        printf("Not supported");
        return CODE_NOT_SUPPORTED;
    default:
        printf("Invalid node type");
        return CODE_LOGIC_ERROR;
    }
}

ResultCode nodeErase(Node *root, const String *key)
{
    Node *node = nodeGet(root, key);
    if (node == NULL)
    {
        printf("Cannot erase. Node %s not found in %s", string_cStr(key), string_cStr(root->key));
        return CODE_LOGIC_ERROR;
    }

    // Get the parent node, which must be an object node
    Node *parent = node->parent;
    if (parent == NULL || parent->type != NODE_TYPE_OBJECT)
    {
        return CODE_LOGIC_ERROR;
    }

    // Check if we can find it among the children
    Vector *objects = (Vector *)parent->data;
    Iterator found = iterator_find(vector_begin(objects), vector_end(objects), &node);
    if (iterator_equal(found, vector_end(objects)))
    {
        return CODE_OK;
    }

    // Free the contents themselves, then remove the element from the list
    if (nodeFree(node) != CODE_OK)
    {
        return CODE_MEMORY_ERROR;
    }
    if (vector_erase(objects, found, iterator_increase(found, 1)) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    return CODE_OK;
}

ResultCode nodeFree(void *node)
{
    return CODE_NOT_SUPPORTED;
}

static ResultCode nodeInitialise(Node *node)
{
    return CODE_NOT_SUPPORTED;
}