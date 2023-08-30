#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "write.h"
#include "read.h"
#include "string_type.h"
#include "node.h"
#include "vector.h"
#include "parse.h"

int main(int argc, char **argv)
{
    // Turn the command provided into a complete
    String *command = string_create();
    for (size_t i = 1; i < argc; i++)
    {
        string_joinInPlace(command, string_createFromLiteral(argv[i]));
    }

    // Parse the command
    ParsedCommand *parsedCommand = parse(command);
    if (parsedCommand == NULL)
    {
        return CODE_SYNTAX_ERROR;
    }

    // If the user has provided a filename, we start by loading the JSON
    // inside the file. If not, we begin with an empty JSON
    Node *root;
    if (parsedCommand->filename != NULL)
    {
        root = read_from_file(parsedCommand->filename);
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

    // Decide depending on the type of command
    switch (parsedCommand->command)
    {
    case COMMAND_SET_KEY:
    {
        // Get the path to the original node
        JsonPath originalPath = parsedCommand->data.setKeyData.path;
        // Try to find the node
        Node *node = json_path_get(&originalPath, root);
        if (!node)
        {
            return CODE_LOGIC_ERROR;
        }
        // Once the node has been found, modify the key
        if (node_set_key(node, &parsedCommand->data.setKeyData.key) != CODE_OK)
        {
            return CODE_LOGIC_ERROR;
        }
        return CODE_OK;
    }
    case COMMAND_SET_VALUE:
    {
        // Create a new node with the string that has been provided
        Node *new = read_from_string(&parsedCommand->data.setValueData.value);
        if (!new)
        {
            return CODE_SYNTAX_ERROR;
        }
        // Get the path to the original node
        JsonPath originalPath = parsedCommand->data.setValueData.path;
        // Try to find the node
        // TODO: this has to be done recursively
        Node *node = json_path_get(&originalPath, root);
        if (!node)
        {
            // In this case, the node has to be appended, so first go back one
            // element in the path
            JsonPath *previousPath = json_path_decrease(&originalPath);
            // Try to find this node
            Node *previousNode = json_path_get(previousPath, root);
            if (!previousNode)
            {
                return CODE_LOGIC_ERROR;
            }
            // Get the key of the new node
            String *key = json_path_leaf(&originalPath);
            // Append to the parent
            return node_append(previousNode, key, new);
        }
        else
        {
            return node_set_data(node, new);
        }
        return CODE_OK;
    }
    case COMMAND_ERASE:
    {
        // Get the path of the node we want to erase
        JsonPath path = parsedCommand->data.eraseData.path;
        // Try to find the node in the tree
        Node *node = json_path_get(&path, root);
        if (!node)
        {
            return CODE_MEMORY_ERROR;
        }
        // Remove this node
        return node_erase(node->parent, json_path_leaf(&path));
    }
    }
}
