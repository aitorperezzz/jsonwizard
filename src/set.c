// Includes.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "vector.h"

// Prototypes of static functions.
static int isInteger(const String *string);
static ResultCode initializeData(Node *node);

// Implementation of public functions.

// This function modifies the node indicated by its key.
ResultCode jsonModify(Node *root, const String *key, const String *field, const String *value)
{
    // Check root is available.
    if (root == NULL)
    {
        printf("ERROR: cannot modify node if root is NULL.\n");
        return CODE_ERROR;
    }

    // Try to find the relevant node.
    Node *node = node_get(root, key);
    if (node == NULL)
    {
        printf("ERROR: cannot modify node. Key %s was not found.\n", string_cStr(key));
        return CODE_ERROR;
    }

    // Decide based on the field.
    if (string_compare(field, string_createFromLiteral("type")) == 0)
    {
        return setType(node, typeStringToCode(value));
    }
    else if (string_compare(field, string_createFromLiteral("key")) == 0)
    {
        return setKey(node, value);
    }
    else if (string_compare(field, string_createFromLiteral("data")) == 0)
    {
        return setData(node, value);
    }
    else
    {
        printf("ERROR: cannot modify node. '%s' is not a valid field.\n", string_cStr(field));
        return CODE_ERROR;
    }
}

// Tries to change the type of a node.
ResultCode setType(Node *node, NodeType type)
{
    // Check the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot set type in NULL node.\n");
        return CODE_ERROR;
    }

    // Validate the type provided.
    if (!validateType(type))
    {
        printf("ERROR: cannot change type of node. Type is not valid.\n");
        return CODE_ERROR;
    }

    // First free the data of the node.
    // freeData(node);

    // Set the new type and initialize.
    node->type = type;
    return initializeData(node);
}

// Receives a key and sets it for the node.
ResultCode setKey(Node *node, const String *key)
{
    // Check the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot set key in NULL node.\n");
        return CODE_ERROR;
    }

    // Check a key was provided.
    if (key == NULL)
    {
        printf("ERROR: cannot set key in node. No key was provided.\n");
        return CODE_ERROR;
    }

    // TODO: check the key is not already in use.

    // node->key = string_copy(key);
    return CODE_OK;
}

// Receives data for a node as a string.
// Only available for certain types of nodes.
ResultCode setData(Node *node, const String *value)
{
    // Check the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot set data in NULL node.\n");
        return CODE_ERROR;
    }

    // Check that data was provided.
    if (value == NULL)
    {
        printf("ERROR: cannot set data in node. No data was provided.\n");
        return CODE_ERROR;
    }

    // Check the node is of the correct type.
    switch (node->type)
    {
    case NODE_TYPE_NULL:
        printf("ERROR: cannot set data in a node of type NULL.\n");
        printf("Change its type first.\n");
        return CODE_ERROR;
    case NODE_TYPE_OBJECT:
    case NODE_TYPE_ARRAY:
        printf("ERROR: cannot modify data inside ARRAY and OBJECT nodes with setData.\n");
        return CODE_ERROR;
    case NODE_TYPE_STRING:
        node->data = string_copy(value);
        return CODE_OK;
    case NODE_TYPE_NUMBER:
        if (!isInteger(value))
        {
            printf("ERROR: cannot set data. Not a valid integer value.\n");
            return CODE_ERROR;
        }
        *((int *)node->data) = atoi(string_cStr(value));
        return CODE_OK;
    case NODE_TYPE_BOOLEAN:;
        Boolean boolean = booleanStringToCode(value);
        if (boolean == BOOL_UNKNOWN)
        {
            printf("ERROR: cannot set data. '%s' is not a valid boolean value.\n", string_cStr(value));
            return CODE_ERROR;
        }
        *((Boolean *)node->data) = boolean;
    default:
        printf("ERROR: cannot change data in node of unknown type.\n");
        return CODE_ERROR;
    }
}

// Receives a number as a string and checks if it is valid.
static int isInteger(const String *string)
{
    for (size_t i = 0, n = string_length(string); i < n; i++)
    {
        if (!isdigit(string_at(string, i)))
        {
            return 0;
        }
    }
    return 1;
}

// Presuposes the data of the node is NULL and reserves memory
// according to the type of the node.
static ResultCode initializeData(Node *node)
{
    // Check if the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot reset data of NULL node.\n");
        return CODE_ERROR;
    }

    // Check that the data is NULL, as expected.
    if (node->data != NULL)
    {
        printf("ERROR: cannot initialize data of node. Data is not NULL.\n");
        return CODE_ERROR;
    }

    // Decide depending on the type of node.
    switch (node->type)
    {
    case NODE_TYPE_NULL:
        return CODE_OK;
    case NODE_TYPE_STRING:
        // Create space for a string and initialize it to empty.
        node->data = string_create();
        return CODE_OK;
    case NODE_TYPE_NUMBER:
        // Create space for an integer and store a zero.
        node->data = malloc(sizeof(int));
        *((int *)node->data) = 0;
        return CODE_OK;
    case NODE_TYPE_BOOLEAN:
        // Create space for a boolean (integer) and store unkown.
        node->data = malloc(sizeof(Boolean));
        *((Boolean *)node->data) = BOOL_UNKNOWN;
        return CODE_OK;
    case NODE_TYPE_OBJECT:
        // Initialize an object data structure.
        node->data = vector_create(sizeof(Node *), &node_free_raw);
        return CODE_OK;
    case NODE_TYPE_ARRAY:
        // TODO: reset data in an array node.
        return CODE_ERROR;
    default:
        printf("ERROR: could not reset data in node. Type unknown.\n");
        return CODE_ERROR;
    }
}

// Receives a type code and validates it.
int validateType(NodeType type)
{
    switch (type)
    {
    case NODE_TYPE_NULL:
    case NODE_TYPE_STRING:
    case NODE_TYPE_NUMBER:
    case NODE_TYPE_BOOLEAN:
    case NODE_TYPE_ARRAY:
    case NODE_TYPE_OBJECT:
        return 1;
    default:
        return 0;
    }
}

// Gets the boolean as a string and returns its code.
Boolean booleanStringToCode(const String *string)
{
    if (string_compare(string, string_createFromLiteral("true")) == 0)
    {
        return BOOL_TRUE;
    }
    else if (string_compare(string, string_createFromLiteral("false")) == 0)
    {
        return BOOL_FALSE;
    }
    else
    {
        printf("ERROR. %s is not a valid boolean type.\n", string_cStr(string));
        return BOOL_UNKNOWN;
    }
}

// Gets a boolean code and converts it to a string
// storing it in the buffer provided.
String *booleanCodeToString(Boolean code)
{
    switch (code)
    {
    case BOOL_TRUE:
        return string_createFromLiteral("true");
    case BOOL_FALSE:
        return string_createFromLiteral("false");
    default:
        printf("ERROR: boolean code is not valid.\n");
        return NULL;
    }
}

// Receives a string with the type of node.
// and tries to convert to the type code.
NodeType typeStringToCode(const String *string)
{
    if (string == NULL)
    {
        printf("ERROR: could not get the code for the type. No type provided.\n");
        return NODE_TYPE_NULL;
    }

    // Decide depending on the string.
    if (string_compare(string, string_createFromLiteral("null")) == 0)
    {
        return NODE_TYPE_NULL;
    }
    else if (string_compare(string, string_createFromLiteral("string")) == 0)
    {
        return NODE_TYPE_STRING;
    }
    else if (string_compare(string, string_createFromLiteral("integer")) == 0)
    {
        return NODE_TYPE_NUMBER;
    }
    else if (string_compare(string, string_createFromLiteral("boolean")) == 0)
    {
        return NODE_TYPE_BOOLEAN;
    }
    else if (string_compare(string, string_createFromLiteral("array")) == 0)
    {
        return NODE_TYPE_ARRAY;
    }
    else if (string_compare(string, string_createFromLiteral("object")) == 0)
    {
        return NODE_TYPE_OBJECT;
    }
    else
    {
        printf("ERROR: could not convert string '%s' to a valid type.\n", string_cStr(string));
        return NODE_TYPE_NULL;
    }
}
