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
        return CODE_ERROR;
    }

    // Try to find the relevant node to delete.
    Node *node = searchByKey(*rootAddress, key);
    if (node == NULL)
    {
        printf("ERROR: cannot delete node. Key %s was not found.\n", stringGetBuffer(key));
        return CODE_ERROR;
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
            return CODE_OK;
        }
        else
        {
            printf("ERROR: cannot delete node. Parent was not found.\n");
            return CODE_ERROR;
        }
    }

    // Search the position of this node in the parent's childs.
    Vector *parentData = (Vector *)parent->data;
    size_t i;
    for (i = 0; i < vector_size(parentData); i++)
    {
        if (stringCompare(((Node *)(vector_get(parentData, i)))->key, node->key) == 0)
        {
            break;
        }
    }
    if (i == vector_size(parentData))
    {
        printf("ERROR: could not delete node. Index of child not found.\n");
        return CODE_ERROR;
    }

    // Erase the i-th element in the vector
    Iterator iterator = vector_begin(parentData);
    vector_erase(parentData, iterator_increase(iterator, i), iterator_increase(iterator, i + 1));
    freeNode(node);

    return CODE_OK;
}

// Quits the program.
int jsonQuit(Node *root, Vector *words)
{
    // Free the root structure.
    freeNode(root);

    // Free the command buffer.
    vector_free(words);

    return -1;
}

// Deletes only the data of a node if necessary.
int freeData(Node *node)
{
    // Check the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot free data in NULL node.\n");
        return CODE_ERROR;
    }

    // Check if there is actually some data.
    if (node->data == NULL)
    {
        // Nothing to do.
        return CODE_OK;
    }

    // Now decide according to the type of node.
    switch (node->type)
    {
    case NODE_TYPE_NULL:
        return CODE_OK;
    case NODE_TYPE_STRING:
    case NODE_TYPE_NUMBER:
    case NODE_TYPE_BOOLEAN:
        free(node->data);
        node->data = NULL;
        return CODE_OK;
    case NODE_TYPE_OBJECT:
        // Free the child objects.
        ;
        Vector *data = (Vector *)node->data;
        for (size_t i = 0; i < vector_size(data); i++)
        {
            freeNode((Node *)vector_get(data, i));
        }

        // Free the object data structure itself.
        free(data);
        return CODE_OK;
    case NODE_TYPE_ARRAY:
        // TODO: free the data of an array node.
        return CODE_ERROR;
    default:
        return CODE_ERROR;
    }
}

// Receives a pointer to a node and frees it recursively.
int freeNode(Node *node)
{
    // Check that the node is available.
    if (node == NULL)
    {
        printf("ERROR: cannot free NULL node.\n");
        return CODE_ERROR;
    }

    freeData(node);
    free(node);

    return CODE_ERROR;
}
