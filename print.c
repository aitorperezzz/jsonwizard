// This module writes a JSON object, to a text file or to stdin.

// Include header files.
#include <stdio.h>
#include <string.h>

#include "print.h"

// Defines.
typedef enum lastNode_enum
{
  LAST_YES,
  LAST_NO
} LASTNODE;

// Prototypes of static functions.
static int printNode(FILE *, NODE *, int, int);
static int printBlanks(char *, int);
static int numberOfDigits(int);

// Receives the root of a JSON object and a name for the file.
int jsonPrintToFile(NODE *root, char *filename)
{
  // Concatenate .json to the name of the file.
  char name[STRING_LENGTH + 1];
  memcpy(name, filename, strlen(filename));
  strcpy(&name[strlen(filename)], ".json");

  // Open the file in write mode.
  FILE *file = fopen(name, "w");
  if (file == NULL)
  {
    printf("ERROR: Could not open a file to write to.\n");
    return JSON_ERROR;
  }

  // Print the root node to the file.
  printNode(file, root, 0, LAST_YES);

  // Close the file.
  fclose(file);
  return JSON_OK;
}

// Receives a pointer to the whole JSON and a key, and
// prints to stdin.
int jsonPrintToStdin(NODE *root, char *key)
{
  if (root == NULL)
  {
    printf("ERROR: Cannot print to stdin. Root is NULL.\n");
    return JSON_ERROR;
  }

  if (strcmp(key, "") == 0)
  {
    // The user wants to print the root to stdin.
    return printNode(NULL, root, 0, LAST_YES);
  }
  else
  {
    // Look for the node.
    NODE *parent = searchByKey(root, key);
    if (parent == NULL)
    {
      printf("ERROR. Cannot print to stdin. Key %s was not found.\n", key);
      return JSON_ERROR;
    }

    return printNode(NULL, parent, 0, LAST_YES);
  }
}


// Prints a single node (recursive function).
// If file != NULL, print to file.
// If file == NULL, print to stdin.
static int printNode(FILE *file, NODE *node, int offset, int last)
{
  // Create a buffer for the line to be written.
  int bufferSize = (STRING_LENGTH + 1) * 2 + 4 + offset * 2;
  char buffer[bufferSize];
  memset(buffer, '\0', bufferSize);
  int position = 0;

  // Write the offset to the buffer.
  position = printBlanks(buffer, offset);

  // Write the key to the buffer if the node is not root.
  if (strcmp(node->key, "root") != 0)
  {
    sprintf(&buffer[position], "\"%s\": ", node->key);

    // Update the position to write the value of the node.
    position = position + strlen(node->key) + 4;
  }

  // Decide according to type.
  if (node->type == JSON_NULL)
  {
    sprintf(&buffer[position], "null");
    position = position + 4;
  }
  else if (node->type == JSON_STRING)
  {
    sprintf(&buffer[position], "\"%s\"", (char *) node->data);
    position = position + strlen((char *) node->data);
  }
  else if (node->type == JSON_INTEGER)
  {
    int number = *((int *) node->data);
    sprintf(&buffer[position], "%d", number);
    position = position + numberOfDigits(number);
  }
  else if (node->type == JSON_BOOLEAN)
  {
    char boolean[STRING_LENGTH + 1];
    booleanTypeToString(boolean, *((int *) node->data));
    sprintf(&buffer[position], "%s", boolean);
    position = position + strlen(boolean);
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: print an array.
  }
  else if (node->type == JSON_OBJECT)
  {
    sprintf(&buffer[position], "{\n");
    if (file == NULL)
    {
      printf("%s", buffer);
    }
    else
    {
      fprintf(file, "%s", buffer);
    }

    // Call this function recursively on its childs.
    OBJDATA *data = (OBJDATA *) node->data;
    int newLast;
    for (int i = 0; i < data->childNumber; i++)
    {
      newLast = i == data->childNumber - 1 ? LAST_YES : LAST_NO;
      printNode(file, data->objectPointers[i], offset + 1, newLast);
    }

    // Write in buffer the last line of an object node.
    memset(buffer, '\0', bufferSize);
    position = printBlanks(buffer, offset);
    strcpy(&buffer[position], "}");
    position++;
  }
  else
  {
    return JSON_ERROR;
  }

  // Print the buffer as it is right now.
  if (file == NULL)
  {
    // Print it to stdin.
    if (last == LAST_YES)
    {
      printf("%s\n", buffer);
    }
    else
    {
      printf("%s,\n", buffer);
    }
  }
  else
  {
    // Print it to the file.
    if (last == LAST_YES)
    {
      fprintf(file, "%s\n", buffer);
    }
    else
    {
      fprintf(file, "%s,\n", buffer);
    }
  }

  return JSON_OK;
}


// Writes blank spaces to a string.
// Returns the number of characters written.
static int printBlanks(char *string, int number)
{
  memset(string, ' ', 2 * number);
  return 2 * number;
}


// Receives an integer and returns the number of digits it has.
static int numberOfDigits(int n)
{
  int result = 1;
  while (n > 9)
  {
    n = n / 10;
    result++;
  }

  return result;
}
