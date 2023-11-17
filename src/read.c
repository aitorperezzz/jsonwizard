// This module receives the name of a .json file and tries to import it
// to a node structure.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read.h"
#include "types/types_vector.h"
#include "node.h"

Node *read_from_file(const String *filename)
{
    return NULL;
}

Node *read_from_string(const String *string)
{
    return NULL;
}

#if 0

// Defines.
#define JSONSTRING_BLOCK_LENGTH 100

static String *jsonFileToString(FILE *file);
static Node *jsonParse(const String *jsonString);
static Node *parseNode(const String *nodeString);
static size_t lengthOfEscapedString(const char *start, const size_t size);
static size_t getSizeOfNextNode(const char *buffer, const size_t size);
static size_t getSizeOfObjectValue(const char *buffer, const size_t bufferSize);

// Implementation of public functions.

// Receives the current root address and a filename, and tries to load
// a JSON from that file.
// If ok, loads in root and returns ok.
// If not ok returns error and root is new.
ResultCode jsonLoad(Node **rootAddress, String *filename)
{
    // Free the previous root structure and start a new one.
    node_free(*rootAddress);

    // Try to open the specified file.
    FILE *file = fopen(string_cStr(filename), "r");
    if (file == NULL)
    {
        printf("ERROR. could not open the file %s.\n", string_cStr(filename));
        // fclose(file);
        *rootAddress = createRoot();
        return CODE_ERROR;
    }

    // Try to convert it to a string.
    String *jsonString = jsonFileToString(file);
    if (jsonString == NULL)
    {
        printf("ERROR: could not parse the file to a string.\n");
        fclose(file);
        *rootAddress = createRoot();
        return CODE_ERROR;
    }

    // Print the string obtained.
    printf("JSON string obtained from file %s:\n", string_cStr(filename));
    printf("%s\n", string_cStr(jsonString));

    // Now we have a string with the JSON content. Try to parse it.
    Node *newRoot = jsonParse(jsonString);
    if (newRoot == NULL)
    {
        printf("ERROR: the JSON string could not be parsed.\n");
        fclose(file);
        free(jsonString);
        // free(newRoot);
        *rootAddress = createRoot();
        return CODE_ERROR;
    }
    else
    {
        // Make root point to the new root and finish.
        *rootAddress = newRoot;
        free(jsonString);
        printf("JSON loaded to memory:\n");
        print_to_stdout(newRoot);
        fclose(file);
        return CODE_OK;
    }
}

// Implementation of static functions.

// Receives a file pointer and parses it to a string,
// removing spaces and newline characters out of strings.
// If ok, returns a pointer to the string.
// If not ok, frees the string and returns NULL.
static String *jsonFileToString(FILE *file)
{
    // Create a dynamic buffer for the string.
    char *buffer = calloc(JSONSTRING_BLOCK_LENGTH, sizeof(char));
    int bufferLength = JSONSTRING_BLOCK_LENGTH;

    // Copy all the file to a string, as it is.
    int position = 0;
    char character;
    while ((character = fgetc(file)) != EOF)
    {
        buffer[position] = character;
        position++;

        // Check if we have space.
        if (position == bufferLength)
        {
            buffer = realloc(buffer, bufferLength + JSONSTRING_BLOCK_LENGTH);
            if (buffer == NULL)
            {
                printf("ERROR: Not enough memory to store the JSON as a string.\n");
                return NULL;
            }
            bufferLength += JSONSTRING_BLOCK_LENGTH;
        }
    }
    buffer[position] = '\0';

    // Keep track of the final length of the buffer.
    int finalLength = position;

    // Now remove all unnecessary characters (spaces and newlines).
    int size;
    position = 0;
    while (position < finalLength)
    {
        if (buffer[position] == '\"')
        {
            // A new string begins. Calculate the length of the string and jump over.
            size = lengthOfEscapedString(buffer + position + 1, finalLength - position - 1);
            position = position + size + 2;
            continue;
        }
        else if (buffer[position] == ' ' || buffer[position] == '\n')
        {
            // Copy all characters to the left.
            for (int i = position; i < finalLength; i++)
            {
                buffer[i] = buffer[i + 1];
            }
            finalLength--;
            continue;
        }
        position++;
    }

    String *result = string_createFromBuffer(buffer, strlen(buffer));
    free(buffer);
    return result;
}

// Receives a JSON string and tries to parse it to a node structure.
// If ok, returns a pointer to the structure.
// If not ok, returns NULL.
static Node *jsonParse(const String *string)
{
    // Add "root": to the string.
    String *rootString = string_createFromLiteral("\"root\":");
    const String *nodeString = string_join(rootString, string);

    // Parse the root node.
    Node *root = parseNode(nodeString);
    if (root == NULL)
    {
        printf("ERROR: could not parse the JSON string.\n");
        return NULL;
    }

    return root;
}

// Receives a pointer to a buffer where a node is represented,
// and the length of the buffer.
// If ok, returns a node pointer.
// If not ok, frees memory and returns NULL.
static Node *parseNode(const String *string)
{
    size_t position;
    const size_t length = string_length(string);

    // The first thing in the node is the key. Find its length.
    size_t keyLength = (string_cStr(string), length);

    // Store the key.
    String *key = string_createFromBuffer(string_cStr(string), keyLength);

    // Get the position of the value in the node.
    position = keyLength + 3;

    // Create some space for the node.
    Node *node = createNode();
    setKey(node, key);

    // Access the character in position to get the type.
    int valueLength;
    if (string_at(string, position) == 'n')
    {
        // This is a null node. Nothing to do, the node is already null.
    }
    else if (string_at(string, position) == '\"')
    {
        // This is a string node.
        valueLength = length - position - 2;
        String *value = string_createFromBuffer(string_cStr(string) + position + 1, valueLength);

        // Update node information.
        setType(node, NODE_TYPE_STRING);
        setData(node, value);
    }
    else if (isdigit(string_at(string, position)))
    {
        // This is an integer node.
        valueLength = length - position;
        String *numberString = string_createFromBuffer(string_cStr(string) + position, valueLength);

        // Update node information.
        setType(node, NODE_TYPE_NUMBER);
        setData(node, numberString);
    }
    else if (string_at(string, position) == 't' || string_at(string, position) == 'f')
    {
        // This is a boolean node.
        setType(node, NODE_TYPE_BOOLEAN);
        if (string_at(string, position) == 't')
        {
            setData(node, string_createFromLiteral("true"));
        }
        else if (string_at(string, position) == 'f')
        {
            setData(node, string_createFromLiteral("false"));
        }
    }
    else if (string_at(string, position) == '[')
    {
        // This is an array node.
        printf("ERROR: the program does not yet support array nodes.\n");
        free(node);
        return NULL;
    }
    else if (string_at(string, position) == '{')
    {
        // This is an object node. Set the type.
        setType(node, NODE_TYPE_OBJECT);

        // Variables regarding the object.
        int pointer = position + 1;
        // int objectSize = length - objectBegin - 1;
        //  Position of the last curly brace.
        int objectEnd = length - 1;

        // Get the size of this object.
        // int valueSize = getSizeOfObjectValue(&string[pointer + 1], length - pointer - 1);

        // Parse the object looking for the child nodes.
        Vector *data = (Vector *)node->data;
        Node *newNode;
        // int pointer = objectBegin;
        size_t childSize;
        // pointer++;
        // int endOfObject = pointer + objectSize - 2;
        while (pointer < objectEnd)
        {
            childSize = getSizeOfNextNode(string_cStr(string) + pointer, objectEnd - pointer);
            if (childSize > 0)
            {
                // A new node exists.
                String *nodeString = string_createFromBuffer(string_cStr(string) + pointer, childSize);

                newNode = parseNode(nodeString);
                if (newNode == NULL)
                {
                    printf("ERROR: could not create node beginning at position %d.\n", pointer);
                    printf("ERROR: string received by parseNode: %s.\n", string_cStr(string) + pointer);
                    free(node);
                    return NULL;
                }

                // Add the new node to the child array.
                newNode->parent = node;
                vector_push(data, newNode);

                // Prepare for the next node.
                pointer = pointer + childSize;
                if (string_at(string, position) == ',')
                {
                    // We have to skip one position for the next node.
                    pointer++;
                }
                else if (pointer == length)
                {
                    // We have finished.
                    break;
                }
            }
        }
    }
    else
    {
        // Error parsing the node.
        printf("ERROR. value in node could not be recognised.\n");
        printf("ERROR: string received by parseNode: %s.\n", string_cStr(string));
        free(node);
        return NULL;
    }

    // The node has been parsed correctly, so return it.
    return node;
}

// Receives a buffer and its length and tries to find the length of the string.
// The buffer begins with the first character in the string.
// Returns the length of the string, without the "" characters.
static size_t lengthOfEscapedString(const char *buffer, const size_t size)
{
    size_t position = 0;
    while (position < size)
    {
        if (buffer[position] == '\\')
        {
            // The next character is escaped, so skip one.
            position++;
        }
        else if (buffer[position] == '\"')
        {
            // Reached the end of the string.
            return position;
        }
        position++;
    }

    return position;
}

// Receives a pointer to a buffer of chars and returns how long an int is.
/*
static int sizeOfNumber(char *string, int length)
{
  int position = 0;
  while (position < length && isdigit(string[position]))
  {
    position++;
  }

  return position;
}*/

// Receives a buffer of chars and returns the number of characters read until
// the end of the first complete node.
// Returns -1 if anything went wrong.
static size_t getSizeOfNextNode(const char *buffer, const size_t bufferSize)
{
    // Find the length of the key.
    size_t keyLength = lengthOfEscapedString(buffer + 1, bufferSize - 1);
    size_t position = keyLength + 3;

    // Calculate the size of the value.
    size_t size;
    if (buffer[position] == '{')
    {
        size = getSizeOfObjectValue(buffer + position + 1, bufferSize - position - 1);
    }
    else if (buffer[position] == '\"')
    {
        size = lengthOfEscapedString(buffer + position + 1, bufferSize - position - 1) + 2;
    }
    else if (buffer[position] == '[')
    {
        printf("ERROR: cannot calculate size of array node.\n");
        return -1;
    }
    else
    {
        // Find the next comma or the end of the buffer.
        size_t counter = position;
        while (counter < bufferSize && buffer[counter] != ',')
        {
            counter++;
        }
        size = counter - position;
    }

    // Return the size of the node, without the , character.
    return position + size;
}

// Receives a pointer to a buffer of chars and returns the size of the object
// within the curly braces.
static size_t getSizeOfObjectValue(const char *buffer, const size_t bufferSize)
{
    // There is one object open at the moment.
    int open = 1;
    int pointer = 0;
    char c;

    while (pointer < bufferSize)
    {
        c = buffer[pointer];

        // Check if the next character is escaped.
        if (c == '\\')
        {
            pointer = pointer + 2;
            continue;
        }

        // Decide depending on the character.
        if (c != '{' && c != '}')
        {
            pointer++;
            continue;
        }
        else if (c == '{')
        {
            // An object is being opened.
            open++;
            pointer++;
            continue;
        }
        else if (c == '}')
        {
            if (open == 1)
            {
                // The original object is being closed right here.
                return pointer + 2;
            }
            else
            {
                open--;
                pointer++;
                continue;
            }
        }
    }

    // If it reached the end, return the size of the whole block.
    return bufferSize;
}

#endif
