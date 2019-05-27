// This module receives a .json file and tries to import it
// to a node_t structure.

// Includes.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load.h"
#include "set.h"
#include "print.h"
#include "free.h"

// Defines.
#define STRING_BLOCK_LENGTH 100

// Prototypes for static functions.
static char *jsonFileToString(FILE *);
static node_t *jsonParse(char *);
static node_t *parseNode(char *, int);
static int lengthOfString(char *, int);
static int sizeOfNumber(char *, int);
static int getSizeOfNextNode(char *, int);
static int getSizeOfObjectValue(char *, int);


// Receives the current root address and a filename, and tries to load
// a JSON from that file.
// If ok, loads in root and returns ok.
// If not ok, frees, root points to NULL and returns error.
int jsonLoad(node_t **rootAddress, char *filename)
{
  // First free the previous root structure.
  jsonDelete(rootAddress, "root");

  // Try to open the specified file.
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("ERROR. Could not open the specified file.\n");
    fclose(file);
    *rootAddress = NULL;
    return JSON_ERROR;
  }

  // Try to convert it to a string.
  char *jsonString = jsonFileToString(file);
  if (jsonString == NULL)
  {
    printf("ERROR. Could not parse the file to a string.\n");
    fclose(file);
    *rootAddress = NULL;
    return JSON_ERROR;
  }

  // Print the string obtained.
  printf("JSON string obtained from file %s:\n", filename);
  printf("%s\n", jsonString);

  // Now we have a string with the JSON content. Try to parse it.
  node_t *newRoot = jsonParse(jsonString);
  if (newRoot == NULL)
  {
    printf("ERROR: The JSON string could not be parsed.\n");
    fclose(file);
    free(jsonString);
    free(newRoot);
    *rootAddress = NULL;
    return JSON_ERROR;
  }
  else
  {
    // Make root point to the new root and finish.
    *rootAddress = newRoot;
    free(jsonString);
    printf("JSON loaded to memory:\n");
    jsonPrintToStdin(newRoot, "root");
    fclose(file);
    return JSON_OK;
  }
}


// Receives a file pointer and parses it to a string,
// removing spaces and newline characters out of strings.
// If ok, returns a pointer to the string.
// If not ok, frees the string and returns NULL.
static char *jsonFileToString(FILE *file)
{
  // Create a dynamic buffer for the string.
  char *string = calloc(STRING_BLOCK_LENGTH, sizeof(char));
  int length = STRING_BLOCK_LENGTH;

  // Copy all the file to a string as it is.
  int position = 0;
  char character;
  while ((character = fgetc(file)) != EOF)
  {
    string[position] = character;
    position++;

    // Check if we have space.
    if (position == length)
    {
      string = realloc(string, length + STRING_BLOCK_LENGTH);
      length = length + STRING_BLOCK_LENGTH;
      if (string == NULL)
      {
        printf("ERROR: Not enough memory to store the JSON as a string.\n");
        free(string);
        return NULL;
      }
    }
  }
  string[position] = '\0';

  // Now remove all unnecessary characters (spaces and newlines).
  int size;
  position = 0;
  while (position < length)
  {
    if (string[position] == '\"')
    {
      // A new string begins. Calculate the size and jump over.
      size = lengthOfString(&string[position + 1], length - position - 1);
      position = position + size + 2;
      continue;
    }
    else if (string[position] == ' ' || string[position] == '\n')
    {
      // Skip these characters.
      for (int i = position; i < length - 1; i++)
      {
        string[i] = string[i + 1];
      }
      continue;
    }
    position++;
  }

  return string;
}


// Receives a JSON string and tries to parse it to a structure.
// If ok, returns a pointer to the structure.
// If not ok, returns NULL.
static node_t *jsonParse(char *string)
{
  // Add "root": to the string.
  char *rootString = "\"root\":";
  int size = strlen(rootString) + strlen(string) + 1;
  char node_tString[size];
  memcpy(node_tString, rootString, strlen(rootString));
  memcpy(&node_tString[strlen(rootString)], string, strlen(string));
  node_tString[size - 1] = '\0';

  // Parse the root node.
  node_t *root = parseNode(node_tString, size - 1);
  if (root == NULL)
  {
    printf("ERROR. Could not parse the JSON string.\n");
    return NULL;
  }

  return root;
}


// Receives a pointer to a buffer where a node is represented,
// and the length of that node.
// If ok, returns a node pointer.
// If not ok, frees memory and returns NULL.
static node_t *parseNode(char *string, int length)
{
  int position;

  // The first thing in the node_t is the key. Find its length.
  int keyLength = lengthOfString(&string[1], length - 1);

  // Store the key.
  char key[STRING_LENGTH + 1];
  memcpy(key, string + 1, keyLength);
  key[keyLength] = '\0';

  // Get the position of the value in the node.
  position = keyLength + 3;

  // Create some space for the node.
  node_t *node = malloc(sizeof(node));
  initializeNode(node);
  setKey(node, key);

  // Access the character in position to get the type.
  if (string[position] == 'n')
  {
    // This is a null node. Nothing to do.
  }
  else if (string[position] == '\"')
  {
    // This is a string node_t. Get the string value.
    int valueLength = lengthOfString(&string[position + 1], length - position - 1);
    char value[STRING_LENGTH + 1];
    memcpy(value, &string[position + 1], valueLength);
    value[valueLength] = '\0';

    // Update node information.
    setType(node, TYPE_STRING);
    setData(node, value);
  }
  else if (isdigit(string[position]))
  {
    // This is an integer node_t. Get the length of the number.
    int numberLength = sizeOfNumber(&string[position], length - position);
    char numberString[STRING_LENGTH + 1];
    memcpy(numberString, &string[position], numberLength);
    numberString[numberLength] = '\0';

    // Update node information.
    setType(node, TYPE_INTEGER);
    setData(node, numberString);
  }
  else if (string[position] == 't' || string[position] == 'f')
  {
    // This is a boolean node.
    setType(node, TYPE_BOOLEAN);

    if (string[position] == 't')
    {
      setData(node, "true");
    }
    else if (string[position] == 'f')
    {
      setData(node, "false");
    }
  }
  else if (string[position] == '[')
  {
    // This is an array node.
    printf("ERROR: the program does not support array nodes.\n");
    free(node);
    return NULL;
  }
  else if (string[position] == '{')
  {
    // This is an object node. Set the type.
    setType(node, TYPE_OBJECT);
    int pointer = position + 1;

    // Parse the object childs looking for nodes.
    objData_t *data = (objData_t *) node->data;
    node_t *newNode;
    int size;
    while (pointer < length)
    {
      size = getSizeOfNextNode(&string[pointer], length - pointer);
      if (size > 0)
      {
        // A new node exists.
        newNode = parseNode(&string[pointer], size);
        if (newNode == NULL)
        {
          printf("ERROR: could not create node beginning at position %d.\n", pointer);
          printf("ERROR: string received by parseNode: %s.\n", &string[pointer]);
          free(node);
          return NULL;
        }

        // Add the new node to the child array.
        newNode->parent = node;
        data->objects[data->childNumber] = newNode;
        setChildNumber(node, data->childNumber++);

        // Prepare for the next node.
        pointer = pointer + size + 1;
      }
    }
  }
  else
  {
    // Error parsing the node.
    printf("ERROR. value in node could not be recognised.\n");
    printf("ERROR: string received by parseNode: %s.\n", string);
    free(node);
    return NULL;
  }

  // The node has been parsed correctly, so return it.
  return node;
}


// Receives a buffer and its length and tries to find the length of the string.
// The buffer begins with the first character in the string.
// Returns the length of the string, without the "" characters.
static int lengthOfString(char *buffer, int length)
{
  int position = 0;

  while (position < length)
  {
    if (buffer[position] == '\"')
    {
      if (buffer[position - 1] != '\\')
      {
        // The string is being closed.
        return position;
      }
      else if (buffer[position - 2] == '\\')
      {
        // Also closed.
        return position;
      }
    }
    position++;
  }

  return position;
}


// Receives a pointer to a buffer of chars and returns how long an int is.
static int sizeOfNumber(char *string, int length)
{
  int position = 0;
  while (position < length && isdigit(string[position]))
  {
    position++;
  }

  return position;
}

// Receives a buffer of chars and returns the number of characters read until
// the end of the next complete node_t.
// Returns -1 if anything went wrong.
static int getSizeOfNextNode(char *string, int length)
{
  // Find the length of the key.
  int keyLength = lengthOfString(&string[1], length - 1);
  int position = keyLength + 3;

  // ?????
  if (position == length)
  {
    return -1;
  }

  // Calculate the size of the value.
  int size;
  if (string[position] == '{')
  {
    size = getSizeOfObjectValue(&string[position], length - position);
  }
  else if (string[position] == '\"')
  {
    size = lengthOfString(&string[position + 1], length - position - 1) + 2;

  }
  else if (string[position] == '[')
  {
    printf("ERROR: cannot calculate size of array node.\n");
    return -1;
  }
  else
  {
    // Find the next comma or the end of the buffer.
    int counter = position;
    while (counter < length && string[counter] != ',')
    {
      counter++;
    }

    size = counter - position;
  }

  // Add the : char and the , char and you get the size of the node_t.
  return position + size;
}

// Receives a pointer to a buffer of chars and returns the size of the object
// with the curly braces.
static int getSizeOfObjectValue(char *string, int length)
{
  // There is one object open at the moment.
  int open = 0;
  int pointer = 0;
  char c;

  while (pointer < length)
  {
    c = string[pointer];
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
        return pointer + 1;
      }
      else
      {
        open--;
        pointer++;
      }
    }
  }
}
