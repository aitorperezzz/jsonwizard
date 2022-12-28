#include "wizard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "print.h"
#include "load.h"
#include "set.h"
#include "free.h"
#include "string_type.h"
#include "node.h"
#include "vector.h"

String *readUserInput();

// Main.
int main(int argc, char **argv)
{
    // String to store the complete input from the user
    String *input = NULL;

    // Separately store the words of the last user command
    Vector *words = vectorCreate(VECTOR_TYPE_STRING);

    // Create the root node of the tree
    Node *root = createRoot();
    if (root == NULL)
    {
        printf("ERROR: could not create root node");
        return -1;
    }

    // Print hwo to use the application
    printHelp();

    // Listen to commands in a loop
    while (1)
    {
        // Print the prompt
        printf("\tjsonwizard> ");

        // Read all input provided by the user
        stringFree(input);
        input = readUserInput();
        if (input == NULL)
        {
            printf("ERROR: could not read user input");
            return -1;
        }

        // Parse input into words
        if (vectorClear(words) != JSON_OK)
        {
            printf("Could not clear last user command");
            return -1;
        }
        if (parseCommand(input, words) == JSON_ERROR)
        {
            printf("ERROR: there was an error parsing the command. Continuing...\n");
            continue;
        }

        // Execute the command and treat the errors.
        ResultCode result = executeCommand(words, &root);
        if (result == JSON_OK)
        {
            printf("The command executed correctly. Continuing...\n");
            continue;
        }
        else if (result == JSON_ERROR)
        {
            printf("The command could not be executed. Continuing...\n");
            continue;
        }
        else if (result == JSON_QUIT)
        {
            jsonQuit(root, words);
            printf("Exiting the program...\n");
            break;
        }
    }
}

// Prints the help menu.
ResultCode printHelp()
{
    printf("How to use jsonwizard:\n");
    printf("\t> append <child key> to <parent key>\n");
    printf("\t> modify <key> set <field> <value>\n");
    printf("\t> delete [<key>]\n");
    printf("\t> write <filename without extension>\n");
    printf("\t> print [<key>]\n");
    printf("\t> load <filename>\n");
    printf("\t> help\n");
    printf("\t> quit\n");

    return JSON_OK;
}

String *readUserInput()
{
    String *input = stringCreate();
    // Read in chunks of 256 characters until the \n character is found
    const size_t chunkSize = 256;
    size_t index = 0;
    do
    {
        if (stringReserve(input, input->capacity + chunkSize) != JSON_OK)
        {
            return NULL;
        }
        if (fgets(input->buffer, chunkSize, stdin) == NULL)
        {
            return NULL;
        }
        index += chunkSize;
    } while (strchr(input->buffer + index, '\n') != NULL);
    return input;
}

ResultCode parseCommand(const String *input, Vector *words)
{
    // General checks
    if (input == NULL)
    {
        return JSON_MEMORY_ERROR;
    }
    if (words == NULL)
    {
        return JSON_MEMORY_ERROR;
    }

    // Counter for letters.
    size_t charBegin = 0;

    for (size_t i = 0, n = stringLength(input); i < n; i++)
    {
        if (input->buffer[i] == ' ' || i == n - 1)
        {
            String *newWord = stringCreate();
            stringCopyFromBuffer(newWord, input->buffer + charBegin, i - charBegin);
            vectorAdd(words, (void *)newWord, sizeof(newWord));
            continue;
        }
    }
    return JSON_OK;
}

// Execute the command.
ResultCode executeCommand(const Vector *command, Node **rootAddress)
{
    if (strcmp(vectorGet(command, 0), "append") == 0)
    {
        // Structure: append <child key> to <parent key>.
        return jsonAppend(rootAddress, vectorGet(command, 3), vectorGet(command, 1));
    }
    else if (strcmp(vectorGet(command, 0), "modify") == 0)
    {
        // Structure: modify <key> set <field> <value>.
        return jsonModify(*rootAddress, vectorGet(command, 1), vectorGet(command, 3), vectorGet(command, 4));
    }
    else if (strcmp(vectorGet(command, 0), "delete") == 0)
    {
        // Structure: delete [<key>].
        return jsonDelete(rootAddress, vectorGet(command, 1));
    }
    else if (strcmp(vectorGet(command, 0), "write") == 0)
    {
        // Structure: write <filename>
        return printToFile(*rootAddress, vectorGet(command, 1));
    }
    else if (strcmp(vectorGet(command, 0), "print") == 0)
    {
        // Structure: print [<key>].
        return printToStdin(*rootAddress, vectorGet(command, 1));
    }
    else if (strcmp(vectorGet(command, 0), "load") == 0)
    {
        // Structure: load <filename>.
        return jsonLoad(rootAddress, vectorGet(command, 1));
    }
    else if (strcmp(vectorGet(command, 0), "quit") == 0)
    {
        return JSON_QUIT;
    }
    else if (strcmp(vectorGet(command, 0), "help") == 0)
    {
        return printHelp();
    }
    else
    {
        printf("ERROR: the command could not be understood.\n");
        return JSON_ERROR;
    }
}

// Tries to create a node and returns a pointer to that node.
Node *createNode(void)
{
    // Try to create space for a new node.
    Node *node = malloc(sizeof(Node));
    if (node == NULL)
    {
        printf("ERROR: could not create the node.\n");
        return NULL;
    }

    // Initialize it to default values.
    if (initializeNode(node) == JSON_OK)
    {
        return node;
    }

    return NULL;
}

// Tries to create a root node.
Node *createRoot(void)
{
    Node *root = createNode();
    if (root == NULL)
    {
        printf("ERROR: could not create the root node.\n");
        return NULL;
    }

    setKey(root, stringCreateFromLiteral("root"));
    setType(root, NODE_TYPE_OBJECT);
    return root;
}

// Initialize a node to the default values.
ResultCode initializeNode(Node *node)
{
    if (node == NULL)
    {
        printf("ERROR: cannot initialize a NULL node.\n");
        return JSON_ERROR;
    }

    node->type = NODE_TYPE_NULL;
    node->key = stringCreateFromLiteral("");
    node->parent = NULL;
    node->data = NULL;

    return JSON_OK;
}

// Append a node to a parent node.
ResultCode jsonAppend(Node **rootAddress, const String *parentKey, const String *childKey)
{
    Node *root = *rootAddress;

    if (root == NULL)
    {
        printf("ERROR: cannot append node because root is NULL.\n");
        return JSON_ERROR;
    }

    // Try to find the parent node.
    Node *parent = searchByKey(root, parentKey);
    if (parent == NULL)
    {
        printf("ERROR: could not find the parent key in the root node.\n");
        return JSON_ERROR;
    }
    else if (validateType(parent->type))
    {
        printf("ERROR: cannot append node to a node without a valid type.\n");
        return JSON_ERROR;
    }
    else if (parent->type != NODE_TYPE_OBJECT)
    {
        printf("ERROR: cannot append node to a node that is not of type object.\n");
        return JSON_ERROR;
    }

    // TODO: prevent from appending when the key is already found.

    // Only in this case create a new node.
    Node *newNode = createNode();
    if (newNode == NULL)
    {
        printf("ERROR: could not create a child node.\n");
        return JSON_ERROR;
    }
    setKey(newNode, childKey);

    // Link the parent to the child.
    vectorAdd((Vector *)parent->data, newNode, sizeof(newNode));
    newNode->parent = parent;

    return JSON_OK;
}

// Tries to find, recursively, a node with the specified key.
// If found, returns a pointer to that node.
Node *searchByKey(Node *node, const String *key)
{
    if (node == NULL)
    {
        printf("ERROR: cannot find key in NULL node.\n");
        return NULL;
    }

    if (stringCompare(node->key, key) == 0)
    {
        // This node has the key.
        return node;
    }
    else if (node->type == NODE_TYPE_OBJECT)
    {
        // Look recursively in this case.
        Node *result;
        Vector *data = (Vector *)node->data;
        for (size_t i = 0, n = vectorSize(data); i < n; i++)
        {
            Node *element = (Node *)vectorGet(data, i);
            if (element != NULL)
            {
                result = searchByKey(element, key);
                if (result != NULL)
                {
                    return result;
                }
            }
        }

        // I have searched inside this object and the key was not found.
        return NULL;
    }
    else
    {
        // This node does not have the key and is not
        // a searchable node.
        return NULL;
    }
}
