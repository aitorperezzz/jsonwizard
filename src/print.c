#include "print.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "wizard.h"
#include "set.h"

static ResultCode printNode(FILE *file, const Node *node, size_t offset, int isLast);
static ResultCode addBlanks(String *string, size_t number);

// Receives the root of a JSON object and a name for the file.
ResultCode printToFile(const Node *root, const String *filename)
{
    // Concatenate .json to the name of the file.
    String *name = string_join(filename, string_createFromLiteral(".json"));

    // Open the file in write mode.
    FILE *file = fopen(string_cStr(name), "w");
    if (file == NULL)
    {
        printf("ERROR: Could not open a file to write to.\n");
        return CODE_ERROR;
    }

    // Print the root struct Node to the file.
    printNode(file, root, 0, 1);

    // Close the file.
    fclose(file);
    return CODE_OK;
}

// Receives a pointer to the whole JSON and a key, and
// prints to stdin.
ResultCode printToStdin(Node *root, const String *key)
{
    if (root == NULL)
    {
        printf("ERROR: Cannot print to stdin. Root is NULL.\n");
        return CODE_ERROR;
    }

    if (string_compare(key, string_createFromLiteral("")) == 0)
    {
        // The user wants to print the root to stdin.
        return printNode(NULL, root, 0, 1);
    }
    else
    {
        // Look for the node.
        Node *parent = searchByKey(root, key);
        if (parent == NULL)
        {
            printf("ERROR. Cannot print to stdin. Key %s was not found.\n", string_cStr(key));
            return CODE_ERROR;
        }

        return printNode(NULL, parent, 0, 1);
    }
}

// Prints a single node (recursive function).
// If file != NULL, print to file.
// If file == NULL, print to stdin.
static ResultCode printNode(FILE *file, const Node *node, size_t offset, int isLast)
{
    // Check inputs
    if (file == NULL)
    {
        printf("ERROR: file pointer provided is NULL");
        return CODE_MEMORY_ERROR;
    }
    if (node == NULL)
    {
        printf("ERROR: node provided is NULL");
        return CODE_MEMORY_ERROR;
    }

    // Create a buffer for the line to be written.
    String *buffer = string_create();

    // Write the offset to the buffer.
    addBlanks(buffer, offset);

    // Write the key to the buffer if the node is not root.
    if (string_compare(node->key, string_createFromLiteral("root")) != 0)
    {
        string_copy(buffer, string_createFromLiteral("\""));
        string_copy(buffer, node->key);
        string_copy(buffer, string_createFromLiteral("\": "));
    }

    // Decide according to type.
    if (node->type == NODE_TYPE_NULL)
    {
        string_copy(buffer, string_createFromLiteral("null"));
    }
    else if (node->type == NODE_TYPE_STRING)
    {
        string_copy(buffer, string_createFromLiteral("\""));
        string_copy(buffer, (String *)node->data);
        string_copy(buffer, string_createFromLiteral("\""));
    }
    else if (node->type == NODE_TYPE_NUMBER)
    {
        int number = *((int *)node->data);
        char numberBuffer[sizeof(char) * (int)log10(number)];
        snprintf(numberBuffer, sizeof(numberBuffer), "%d", number);
        string_copyFromBuffer(buffer, numberBuffer, strlen(numberBuffer));
    }
    else if (node->type == NODE_TYPE_BOOLEAN)
    {
        String *booleanString = booleanCodeToString(*((Boolean *)node->data));
        string_copy(buffer, booleanString);
    }
    else if (node->type == NODE_TYPE_ARRAY)
    {
        // TODO
    }
    else if (node->type == NODE_TYPE_OBJECT)
    {
        // Open the object and write the current buffer contents as this is a
        // recursive function
        string_copy(buffer, string_createFromLiteral("{\n"));
        fprintf(file, "%s", string_cStr(buffer));

        // Call this function recursively on its childs.
        Vector *children = (Vector *)node->data;
        int newLast;
        const size_t size = vector_size(children);
        for (size_t i = 0; i < size; i++)
        {
            newLast = i == size - 1 ? 1 : 0;
            printNode(file, vector_get(children, i), offset + 1, newLast);
        }

        // Write the last line that all object nodes finish with
        string_free(buffer);
        addBlanks(buffer, offset);
        string_copy(buffer, string_createFromLiteral("}"));
    }
    else
    {
        return CODE_ERROR;
    }

    // Print the buffer contents
    if (!isLast)
    {
        string_copy(buffer, string_createFromLiteral(","));
    }
    fprintf(file, "%s\n", string_cStr(buffer));
    return CODE_OK;
}

// Writes blank spaces to a string.
// Returns the number of characters written.
static ResultCode addBlanks(String *string, size_t number)
{
    char blanks[2 * number];
    memset(blanks, ' ', 2 * number);
    string_copyFromBuffer(string, blanks, 2 * number);
    return CODE_OK;
}
