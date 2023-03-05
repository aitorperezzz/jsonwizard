#include "free.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"

// Implementation of public functions.

// Deletes the optionally selected node from the tree.
// If no key is given, the whole tree is deleted.
int jsonDelete(Node **rootAddress, const String *key)
{
    // Check the root node is available.
    if (*rootAddress == NULL)
    {
        printf("ERROR: cannot delete node in a NULL root node.\n");
        return JSON_ERROR;
    }

    // Try to find the relevant node to delete.
    Node *node = searchByKey(*rootAddress, key);
    if (node == NULL)
    {
        printf("ERROR: cannot delete node. Key %s was not found.\n", stringGetBuffer(key));
        return JSON_ERROR;
    }

    // Try to get the parent node.
    Node *parent = node->parent;

    if (parent == NULL)
    {
        if (stringCompare(node->key, stringCreateFromLiteral("root")) == 0)
        {
            // The user wants to delete the root tree.
            freeNode(*rootAddress);
            *rootAddress = createRoot();
            return JSON_OK;
        }
        else
        {
            printf("ERROR: cannot delete node. Parent was not found.\n");
            return JSON_ERROR;
        }
    }

    // Search the position of this node in the parent's childs.
    Vector *parentData = (Vector *)parent->data;
    size_t i;
    for (i = 0; i < vectorSize(parentData); i++)
    {
        if (stringCompare(((Node *)(vectorGet(parentData, i)))->key, node->key) == 0)
        {
            break;
        }
    }
    if (i == vectorSize(parentData))
    {
        printf("ERROR: could not delete node. Index of child not found.\n");
        return JSON_ERROR;
    }

    // Erase the i-th element in the vector
    vectorErase(parentData, i);
    freeNode(node);

    return JSON_OK;
}

// Quits the program.
int jsonQuit(Node *root, Vector *words)
{
    // Free the root structure.
    freeNode(root);

    // Free the command buffer.
    vectorFree(words);

    return JSON_QUIT;
}

// Deletes only the data of a node if necessary.
int freeData(Node *node)
{
    // Check the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot free data in NULL node.\n");
        return JSON_ERROR;
    }

    // Check if there is actually some data.
    if (node->data == NULL)
    {
        // Nothing to do.
        return JSON_OK;
    }

    // Now decide according to the type of node.
    switch (node->type)
    {
    case NODE_TYPE_NULL:
        return JSON_OK;
    case NODE_TYPE_STRING:
    case NODE_TYPE_INTEGER:
    case NODE_TYPE_BOOLEAN:
        free(node->data);
        node->data = NULL;
        return JSON_OK;
    case NODE_TYPE_OBJECT:
        // Free the child objects.
        ;
        Vector *data = (Vector *)node->data;
        for (size_t i = 0; i < vectorSize(data); i++)
        {
            freeNode((Node *)vectorGet(data, i));
        }

        // Free the object data structure itself.
        free(data);
        return JSON_OK;
    case NODE_TYPE_ARRAY:
        // TODO: free the data of an array node.
        return JSON_ERROR;
    default:
        return JSON_ERROR;
    }
}

// Receives a pointer to a node and frees it recursively.
int freeNode(Node *node)
{
    // Check that the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot free NULL node.\n");
        return JSON_ERROR;
    }

    freeData(node);
    free(node);

    return JSON_ERROR;
}
