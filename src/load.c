// This module receives the name of a .json file and tries to import it
// to a node structure.

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
#define JSONSTRING_BLOCK_LENGTH 100

// Prototypes of private functions.
static char *jsonFileToString(FILE *);
static node_t *jsonParse(char *);
static node_t *parseNode(char *, int);
static int lengthOfString(char *, int);
//static int sizeOfNumber(char *, int);
static int getSizeOfNextNode(char *, int);
static int getSizeOfObjectValue(char *, int);


// Implementation of public functions.

// Receives the current root address and a filename, and tries to load
// a JSON from that file.
// If ok, loads in root and returns ok.
// If not ok returns error and root is new.
int jsonLoad(node_t **rootAddress, char *filename)
{
  // Free the previous root structure and start a new one.
  freeNode(*rootAddress);

  // Try to open the specified file.
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("ERROR. could not open the file %s.\n", filename);
    //fclose(file);
    *rootAddress = createRoot();
    return JSON_ERROR;
  }

  // Try to convert it to a string.
  char *jsonString = jsonFileToString(file);
  if (jsonString == NULL)
  {
    printf("ERROR: could not parse the file to a string.\n");
    fclose(file);
    *rootAddress = createRoot();
    return JSON_ERROR;
  }

  // Print the string obtained.
  printf("JSON string obtained from file %s:\n", filename);
  printf("%s\n", jsonString);

  // Now we have a string with the JSON content. Try to parse it.
  node_t *newRoot = jsonParse(jsonString);
  if (newRoot == NULL)
  {
    printf("ERROR: the JSON string could not be parsed.\n");
    fclose(file);
    free(jsonString);
    //free(newRoot);
    *rootAddress = createRoot();
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


// Implementation of static functions.

// Receives a file pointer and parses it to a string,
// removing spaces and newline characters out of strings.
// If ok, returns a pointer to the string.
// If not ok, frees the string and returns NULL.
static char *jsonFileToString(FILE *file)
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
      size = lengthOfString(&buffer[position + 1], finalLength - position - 1);
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

  return buffer;
}


// Receives a JSON string and tries to parse it to a node structure.
// If ok, returns a pointer to the structure.
// If not ok, returns NULL.
static node_t *jsonParse(char *string)
{
  // Add "root": to the string.
  char *rootString = "\"root\":";
  int size = strlen(rootString) + strlen(string) + 1;
  char nodeString[size];
  memcpy(nodeString, rootString, strlen(rootString));
  memcpy(&nodeString[strlen(rootString)], string, strlen(string));
  nodeString[size - 1] = '\0';

  // Parse the root node.
  node_t *root = parseNode(nodeString, size - 1);
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
static node_t *parseNode(char *string, int length)
{
  int position;

  // The first thing in the node is the key. Find its length.
  int keyLength = lengthOfString(&string[1], length - 1);

  // Store the key.
  char key[STRING_LENGTH + 1];
  memcpy(key, string + 1, keyLength);
  key[keyLength] = '\0';

  // Get the position of the value in the node.
  position = keyLength + 3;

  // Create some space for the node.
  node_t *node = createNode();
  setKey(node, key);

  // Access the character in position to get the type.
  int valueLength;
  if (string[position] == 'n')
  {
    // This is a null node. Nothing to do, the node is already null.
  }
  else if (string[position] == '\"')
  {
    // This is a string node.
    valueLength = length - position - 2;
    char value[STRING_LENGTH + 1];
    memcpy(value, &string[position + 1], valueLength);
    value[valueLength] = '\0';

    /*
    valueLength = lengthOfString(&string[position + 1], length - position - 1);
    char value[STRING_LENGTH + 1];
    memcpy(value, &string[position + 1], valueLength);
    value[valueLength] = '\0';*/

    // Update node information.
    setType(node, TYPE_STRING);
    setData(node, value);
  }
  else if (isdigit(string[position]))
  {
    // This is an integer node.
    valueLength = length - position;
    char numberString[STRING_LENGTH + 1];
    memcpy(numberString, &string[position], valueLength);
    numberString[valueLength] = '\0';
    /*
    valueLength = sizeOfNumber(&string[position], length - position);
    char numberString[STRING_LENGTH + 1];
    memcpy(numberString, &string[position], valueLength);
    numberString[valueLength] = '\0';*/

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
    printf("ERROR: the program does not yet support array nodes.\n");
    free(node);
    return NULL;
  }
  else if (string[position] == '{')
  {
    // This is an object node. Set the type.
    setType(node, TYPE_OBJECT);

    // Variables regarding the object.
    int pointer = position + 1;
    //int objectSize = length - objectBegin - 1;
    // Position of the last curly brace.
    int objectEnd = length - 1;

    // Get the size of this object.
    //int valueSize = getSizeOfObjectValue(&string[pointer + 1], length - pointer - 1);

    // Parse the object looking for the child nodes.
    objData_t *data = (objData_t *) node->data;
    node_t *newNode;
    //int pointer = objectBegin;
    int childSize;
    //pointer++;
    //int endOfObject = pointer + objectSize - 2;
    while (pointer < objectEnd)
    {
      childSize = getSizeOfNextNode(&string[pointer], objectEnd - pointer);
      if (childSize > 0)
      {
        // A new node exists.
        newNode = parseNode(&string[pointer], childSize);
        if (newNode == NULL)
        {
          printf("ERROR: could not create node beginning at position %d.\n", pointer);
          printf("ERROR: string received by parseNode: %s.\n", &string[pointer]);
          freeData(node);
          free(node);
          return NULL;
        }

        // Add the new node to the child array.
        newNode->parent = node;
        data->objects[data->childNumber] = newNode;
        setChildNumber(node, data->childNumber + 1);

        // Prepare for the next node.
        pointer = pointer + childSize;
        if (string[pointer] == ',')
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
    printf("ERROR: string received by parseNode: %s.\n", string);
    freeData(node);
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
static int getSizeOfNextNode(char *string, int length)
{
  // Find the length of the key.
  int keyLength = lengthOfString(&string[1], length - 1);
  int position = keyLength + 3;

  // Calculate the size of the value.
  int size;
  if (string[position] == '{')
  {
    size = getSizeOfObjectValue(&string[position + 1], length - position - 1);
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

  // Return the size of the node, without the , character.
  return position + size;
}

// Receives a pointer to a buffer of chars and returns the size of the object
// within the curly braces.
static int getSizeOfObjectValue(char *string, int length)
{
  // There is one object open at the moment.
  int open = 1;
  int pointer = 0;
  char c;

  while (pointer < length)
  {
    c = string[pointer];

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
  return length;

}
