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
    String *name = stringJoin(filename, stringCreateFromLiteral(".json"));

    // Open the file in write mode.
    FILE *file = fopen(stringGetBuffer(name), "w");
    if (file == NULL)
    {
        printf("ERROR: Could not open a file to write to.\n");
        return JSON_ERROR;
    }

    // Print the root struct Node to the file.
    printNode(file, root, 0, 1);

    // Close the file.
    fclose(file);
    return JSON_OK;
}

// Receives a pointer to the whole JSON and a key, and
// prints to stdin.
ResultCode printToStdin(Node *root, const String *key)
{
    if (root == NULL)
    {
        printf("ERROR: Cannot print to stdin. Root is NULL.\n");
        return JSON_ERROR;
    }

    if (stringCompare(key, stringCreateFromLiteral("")) == 0)
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
            printf("ERROR. Cannot print to stdin. Key %s was not found.\n", stringGetBuffer(key));
            return JSON_ERROR;
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
        return JSON_MEMORY_ERROR;
    }
    if (node == NULL)
    {
        printf("ERROR: node provided is NULL");
        return JSON_MEMORY_ERROR;
    }

    // Create a buffer for the line to be written.
    String *buffer = stringCreate();

    // Write the offset to the buffer.
    addBlanks(buffer, offset);

    // Write the key to the buffer if the node is not root.
    if (stringCompare(node->key, stringCreateFromLiteral("root")) != 0)
    {
        stringCopy(buffer, stringCreateFromLiteral("\""));
        stringCopy(buffer, node->key);
        stringCopy(buffer, stringCreateFromLiteral("\": "));
    }

    // Decide according to type.
    if (node->type == NODE_TYPE_NULL)
    {
        stringCopy(buffer, stringCreateFromLiteral("null"));
    }
    else if (node->type == NODE_TYPE_STRING)
    {
        stringCopy(buffer, stringCreateFromLiteral("\""));
        stringCopy(buffer, (String *)node->data);
        stringCopy(buffer, stringCreateFromLiteral("\""));
    }
    else if (node->type == NODE_TYPE_INTEGER)
    {
        int number = *((int *)node->data);
        char numberBuffer[sizeof(char) * (int)log10(number)];
        snprintf(numberBuffer, sizeof(numberBuffer), "%d", number);
        stringCopyFromBuffer(buffer, numberBuffer, strlen(numberBuffer));
    }
    else if (node->type == NODE_TYPE_BOOLEAN)
    {
        String *booleanString = booleanCodeToString(*((Boolean *)node->data));
        stringCopy(buffer, booleanString);
    }
    else if (node->type == NODE_TYPE_ARRAY)
    {
        // TODO
    }
    else if (node->type == NODE_TYPE_OBJECT)
    {
        // Open the object and write the current buffer contents as this is a
        // recursive function
        stringCopy(buffer, stringCreateFromLiteral("{\n"));
        fprintf(file, "%s", stringGetBuffer(buffer));

        // Call this function recursively on its childs.
        Vector *children = (Vector *)node->data;
        int newLast;
        const size_t size = vectorSize(children);
        for (size_t i = 0; i < size; i++)
        {
            newLast = i == size - 1 ? 1 : 0;
            printNode(file, vectorGet(children, i), offset + 1, newLast);
        }

        // Write the last line that all object nodes finish with
        stringReset(buffer);
        addBlanks(buffer, offset);
        stringCopy(buffer, stringCreateFromLiteral("}"));
    }
    else
    {
        return JSON_ERROR;
    }

    // Print the buffer contents
    if (!isLast)
    {
        stringCopy(buffer, stringCreateFromLiteral(","));
    }
    fprintf(file, "%s\n", stringGetBuffer(buffer));
    return JSON_OK;
}

// Writes blank spaces to a string.
// Returns the number of characters written.
static ResultCode addBlanks(String *string, size_t number)
{
    char blanks[2 * number];
    memset(blanks, ' ', 2 * number);
    stringCopyFromBuffer(string, blanks, 2 * number);
    return JSON_OK;
}
