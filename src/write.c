#include <stdio.h>
#include <string.h>
#include <math.h>

#include "write.h"
#include "types/types_vector.h"
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
    String *name = types_string_join(filename, types_string_create_from_literal(".json"));

    // Write the string to the file
    FILE *file = fopen(types_string_c_str(name), "w");
    if (file == NULL)
    {
        printf("ERROR: Could not open a file to write to.\n");
        return CODE_ERROR;
    }
    fputs(types_string_c_str(string), file);
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
    String *buffer = types_string_create();

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
        types_string_join_in_place(buffer, types_string_create_from_literal("null"));
        break;
    }
    case NODE_TYPE_STRING:
    {
        types_string_join_in_place(buffer, types_string_create_from_literal("\""));
        types_string_join_in_place(buffer, (String *)node->data);
        types_string_join_in_place(buffer, types_string_create_from_literal("\""));
        break;
    }
    case NODE_TYPE_NUMBER:
    {
        int number = *((int *)node->data);
        char numberBuffer[(int)(ceil(log10(number)) + 1) * sizeof(char)];
        snprintf(numberBuffer, sizeof(numberBuffer), "%d", number);
        types_string_join_in_place(buffer, types_string_create_from_literal(numberBuffer));
        break;
    }
    case NODE_TYPE_BOOLEAN:
    {
        String *booleanString = booleanCodeToString(*((Boolean *)node->data));
        types_string_join_in_place(buffer, booleanString);
    }
    case NODE_TYPE_ARRAY:
    {
        break;
    }
    case NODE_TYPE_OBJECT:
    {
        // Open the object and write the current buffer contents as this is a
        // recursive function
        buffer = types_string_copy(types_string_create_from_literal("{\n"));

        // Call this function recursively on its childs.
        Vector *children = (Vector *)node->data;
        // int newLast;
        const size_t size = types_vector_size(children);
        for (size_t i = 0; i < size; i++)
        {
            // newLast = i == size - 1 ? 1 : 0;
            // print_node(file, vector_at(children, i), offset + 1, newLast);
        }

        // Write the last line that all object nodes finish with
        types_string_free(buffer);
        add_blanks(buffer, offset);
        buffer = types_string_copy(types_string_create_from_literal("}"));
    }
    }

    // Print the buffer contents
    if (!isLast)
    {
        buffer = types_string_copy(types_string_create_from_literal(","));
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
    return types_string_join_in_place(string, types_string_create_from_literal(blanks));
}
