#include <stdio.h>
#include <string.h>
#include <math.h>

#include "write.h"
#include "vector.h"
#include "set.h"

static ResultCode print_node(FILE *file, const Node *node, size_t offset, int isLast);
static ResultCode add_blanks(String *string, size_t number);

ResultCode write_to_file(const Node *node, const String *filename)
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
    print_node(file, node, 0, 1);

    // Close the file.
    fclose(file);
    return CODE_OK;
}
ResultCode write_to_stdout(const Node *node)
{
    if (node == NULL)
    {
        printf("ERROR: Cannot print to stdin. Root is NULL.\n");
        return CODE_ERROR;
    }

    return print_node(stdout, node, 0, 1);
}

// Prints a single node (recursive function).
// If file != NULL, print to file.
// If file == NULL, print to stdin.
static ResultCode print_node(FILE *file, const Node *node, size_t offset, int isLast)
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
    if (add_blanks(buffer, offset) != CODE_OK)
    {
        return CODE_WRITE_ERROR;
    }

    // Decide according to type
    switch (node->type)
    {
    case NODE_TYPE_NULL:
    {
        string_joinInPlace(buffer, string_createFromLiteral("null"));
        break;
    }
    case NODE_TYPE_STRING:
    {
        string_joinInPlace(buffer, string_createFromLiteral("\""));
        string_joinInPlace(buffer, (String *)node->data);
        string_joinInPlace(buffer, string_createFromLiteral("\""));
        break;
    }
    case NODE_TYPE_NUMBER:
    {
        int number = *((int *)node->data);
        char numberBuffer[(int)(ceil(log10(number)) + 1) * sizeof(char)];
        snprintf(numberBuffer, sizeof(numberBuffer), "%d", number);
        string_joinInPlace(buffer, string_createFromLiteral(numberBuffer));
        break;
    }
    case NODE_TYPE_BOOLEAN:
    {
        String *booleanString = booleanCodeToString(*((Boolean *)node->data));
        string_joinInPlace(buffer, booleanString);
    }
    case NODE_TYPE_ARRAY:
    {
        break;
    }
    case NODE_TYPE_OBJECT:
    {
        // Open the object and write the current buffer contents as this is a
        // recursive function
        buffer = string_copy(string_createFromLiteral("{\n"));
        fprintf(file, "%s", string_cStr(buffer));

        // Call this function recursively on its childs.
        Vector *children = (Vector *)node->data;
        int newLast;
        const size_t size = vector_size(children);
        for (size_t i = 0; i < size; i++)
        {
            newLast = i == size - 1 ? 1 : 0;
            print_node(file, vector_at(children, i), offset + 1, newLast);
        }

        // Write the last line that all object nodes finish with
        string_free(buffer);
        add_blanks(buffer, offset);
        buffer = string_copy(string_createFromLiteral("}"));
    }
    }

    // Print the buffer contents
    if (!isLast)
    {
        buffer = string_copy(string_createFromLiteral(","));
    }
    fprintf(file, "%s\n", string_cStr(buffer));
    return CODE_OK;
}

// Writes blank spaces to a string.
// Returns the number of characters written.
static ResultCode add_blanks(String *string, size_t number)
{
    char blanks[2 * number + 1];
    memset(blanks, ' ', 2 * number);
    blanks[2 * number + 1] = '\0';
    return string_joinInPlace(string, string_createFromLiteral(blanks));
}
