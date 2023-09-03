#include <stdio.h>
#include <string.h>
#include <math.h>

#include "write.h"
#include "vector.h"
#include "set.h"

static String *write_node(const Node *node, size_t offset, bool isLast);
static ResultCode add_blanks(String *string, size_t number);

ResultCode write_to_file(const Node *node, const String *filename)
{
    // Create a string with the contents of the node
    String *string = write_node(node, 0, true);
    if (string == NULL)
    {
        return CODE_LOGIC_ERROR;
    }

    // Concatenate .json to the name of the file.
    String *name = string_join(filename, string_createFromLiteral(".json"));

    // Write the string to the file
    FILE *file = fopen(string_cStr(name), "w");
    if (file == NULL)
    {
        printf("ERROR: Could not open a file to write to.\n");
        return CODE_ERROR;
    }
    fputs(string_cStr(string), file);
    fclose(file);
    return CODE_OK;
}

String *write_to_string(const Node *node)
{
    if (node == NULL)
    {
        printf("ERROR: Cannot print to stdin. Root is NULL.\n");
        return NULL;
    }

    return write_node(node, 0, true);
}

static String *write_node(const Node *node, size_t offset, bool isLast)
{
    // Check inputs
    if (node == NULL)
    {
        printf("ERROR: node provided is NULL");
        return NULL;
    }

    // Create a buffer for the line to be written.
    String *buffer = string_create();

    // Write the offset to the buffer.
    if (add_blanks(buffer, offset) != CODE_OK)
    {
        return NULL;
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

        // Call this function recursively on its childs.
        Vector *children = (Vector *)node->data;
        // int newLast;
        const size_t size = vector_size(children);
        for (size_t i = 0; i < size; i++)
        {
            // newLast = i == size - 1 ? 1 : 0;
            // print_node(file, vector_at(children, i), offset + 1, newLast);
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
    return NULL;
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
