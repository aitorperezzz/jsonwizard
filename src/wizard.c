#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "write.h"
#include "read.h"
#include "types/types_string.h"
#include "node.h"
#include "types/types_vector.h"
#include "parse.h"

ResultCode execute_command(Node *root, const ParsedCommand *parsed_command);
Node *traverse(Node *node, const Vector *steps, const bool create);

int main(int argc, char **argv)
{
    // Turn the command provided into a complete string for the parser
    String *command = types_string_create();
    for (size_t i = 1; i < argc; i++)
    {
        if (types_string_join_in_place(command, types_string_create_from_literal(argv[i])) != CODE_OK)
        {
            return CODE_MEMORY_ERROR;
        }
    }

    // Parse the command
    ParsedCommand *parsed_command = parse(command);
    if (parsed_command == NULL)
    {
        return CODE_SYNTAX_ERROR;
    }
    // Check the json path is valid
    if (!parsed_command->path.valid)
    {
        return CODE_SYNTAX_ERROR;
    }

    // If the user has provided a filename, we start by loading the JSON
    // inside the file. If not, we begin with an empty JSON
    Node *root;
    if (parsed_command->filename != NULL)
    {
        root = read_from_file(parsed_command->filename);
        if (!root)
        {
            return CODE_READ_ERROR;
        }
    }
    else
    {
        root = node_create();
        if (!root)
        {
            return CODE_MEMORY_ERROR;
        }
    }

    // Execute command
    ResultCode result = execute_command(root, parsed_command);
    if (result != CODE_OK)
    {
        return result;
    }

    // Output to file
    return write_to_file(root, parsed_command->filename);
}

ResultCode execute_command(Node *root, const ParsedCommand *parsed_command)
{
    // Decide depending on the type of command
    switch (parsed_command->command)
    {
    case COMMAND_SET_KEY:
    {
        // Get the json path that was provided by the user
        JsonPath json_path = parsed_command->path;
        // Iterate over all the possible paths returned
        for (size_t i = 0, n = types_vector_size(json_path.paths); i < n; i++)
        {
            // Get the current path, which is a vector of steps
            Vector *steps = types_vector_at(json_path.paths, i);
            // Point to the node at the end of the path, in this case
            // we are not allowed to create nodes along the way
            Node *node = traverse(root, steps, false);
            if (!node)
            {
                return CODE_LOGIC_ERROR;
            }
            // Now we are pointing to the node we want, so update the key if possible
            if (node_set_key(node, &parsed_command->data.set_key_data.key) != CODE_OK)
            {
                return CODE_LOGIC_ERROR;
            }
        }
        return CODE_OK;
    }
    case COMMAND_SET_VALUE:
    {
        // First try to create a new ndoe with the string that has been provided
        // Create a new node with the string that has been provided
        Node *new = read_from_string(&parsed_command->data.set_value_data.value);
        if (!new)
        {
            return CODE_SYNTAX_ERROR;
        }
        // Get the json path that was provided by the user
        JsonPath json_path = parsed_command->path;
        // Iterate over all the possible paths returned
        for (size_t i = 0, n = types_vector_size(json_path.paths); i < n; i++)
        {
            // Get the current path, which is a vector of steps
            Vector *steps = types_vector_at(json_path.paths, i);
            // Point to the node at the end of the path, in this case
            // we are not allowed to create nodes along the way
            Node *node = traverse(root, steps, true);
            if (!node)
            {
                return CODE_LOGIC_ERROR;
            }
            // Now we are pointing to the node we want, so set the value
            if (node_set_data(node, new) != CODE_OK)
            {
                return CODE_LOGIC_ERROR;
            }
        }
        return CODE_OK;
    }
    case COMMAND_ERASE:
    {
        // Get the json path that was provided by the user
        JsonPath json_path = parsed_command->path;
        // Iterate over all the possible paths returned
        for (size_t i = 0, n = types_vector_size(json_path.paths); i < n; i++)
        {
            // Get the current path, which is a vector of steps
            Vector *steps = types_vector_at(json_path.paths, i);
            // Point to the node at the end of the path, in this case
            // we are not allowed to create nodes along the way
            Node *node = traverse(root, steps, false);
            if (!node)
            {
                return CODE_LOGIC_ERROR;
            }
            // Now we are pointing to the node we want, so erase it
            if (node_erase(node) != CODE_OK)
            {
                return CODE_LOGIC_ERROR;
            }
        }
        return CODE_OK;
    }
    }
}

Node *traverse(Node *node, const Vector *steps, const bool create)
{
    // Iterate over all the steps in this path
    Node *child = NULL;
    for (size_t j = 0, m = types_vector_size(steps); j < m; j++)
    {
        // Point to the current step
        PathStep *step = types_vector_at(steps, j);
        // Get the child, if possible
        if (step->id == PATH_STEP_INDEX)
        {
            child = node_array_get(node, step->data.index);
        }
        else
        {
            child = node_get(node, &step->data.key);
        }

        if (!child)
        {
            if (create)
            {
                // If the node was not found, we need to create it here
                child = node_create();
                // And then add it to the parent
                if (step->id == PATH_STEP_INDEX)
                {
                    // Create the iterators in order to insert
                    Iterator begin = types_iterator_create(child, sizeof(Node *));
                    Iterator end = types_iterator_increase(begin, 1);
                    Iterator destination = types_iterator_increase(types_vector_begin((Vector *)node->data), step->data.index);
                    node_array_insert(node, begin, end, destination);
                }
                else
                {
                    node_append(node, &step->data.key, child);
                }
            }
            else
            {
                // We are not allowed to create any node, so we have found a step
                // were the path does not lead enywhere
                return NULL;
            }
        }

        // Now we need to point to the child
        node = child;
    }

    // Final node
    return node;
}
