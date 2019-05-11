// This module writes a JSON object, to a text file or to stdin.

// Include header files.
#include "print.h"

// Prototypes of static functions.
static int printNode(FILE *, NODE *, int);
static int printBlanks(char *, int);

// Receives the root of a JSON object and a name for the file.
int jsonPrintToFile(NODE *root, char *filename)
{
  // Concatenate .json to the name of the file.
  char *name = calloc(STRING_LENGTH + 1, sizeof(char));
  for (int i = 0; i < strlen(filename); i++)
  {
    name[i] = filename[i];
  }
  strcpy(&name[strlen(filename)], ".json");

  // Open the file.
  FILE *file = fopen(name, "w");

  if (file == NULL)
  {
    printf("ERROR: The file could not be opened.\n");
    return JSON_ERROR;
  }

  // Free the buffer of the name.
  free(name);

  // Start writing.
  fprintf(file, "{\n");

  // Get the root data.
  OBJDATA *data = (OBJDATA *) root->data;

  for (int i = 0; i < data->childNumber; i++)
  {
    printNode(file, data->objectPointers[i], 1);
  }

  fprintf(file, "}");

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
    printf("{\n");
    OBJDATA *data = (OBJDATA *) root->data;
    for (int i = 0; i < data->childNumber; i++)
    {
      printNode(NULL, data->objectPointers[i], 1);
    }
    printf("}\n");
    return JSON_OK;
  }
  else
  {
    // Print a normal node to stdin.
    NODE *parent = searchByKey(root, key);

    if (parent == NULL)
    {
      printf("ERROR. Cannot print to stdin. Key %s was not found.\n", key);
      return JSON_ERROR;
    }

    return printNode(NULL, parent, 0);
  }
}


// Prints a single node (recursive function).
// If file != NULL, print to file.
// If file == NULL, print to stdin.
static int printNode(FILE *file, NODE *node, int offset)
{
  // Keep a buffer for the line to be written.
  int bufferSize = (STRING_LENGTH + 1) * 2 + 4 + offset;
  char buffer[bufferSize];
  memset(buffer, '\0', bufferSize);

  // Write the offset to the buffer.
  int written = printBlanks(buffer, offset);

  // Write the key to the buffer.
  sprintf(&buffer[written], "\"%s\": ", node->key);

  // Calculate position to write the value.
  int position = written + strlen(node->key) + 4;

  // Decide according to type.
  if (node->type == JSON_NULL)
  {
    sprintf(&buffer[position], "null,\n");
  }
  else if (node->type == JSON_STRING)
  {
    sprintf(&buffer[position], "\"%s\",\n", (char *) node->data);
  }
  else if (node->type == JSON_INTEGER)
  {
    sprintf(&buffer[position], "%d,\n", *((int *) node->data));
  }
  else if (node->type == JSON_BOOLEAN)
  {
    char *boolean = calloc(STRING_LENGTH + 1, sizeof(char));
    booleanTypeToString(boolean, *((int *) node->data));
    sprintf(&buffer[position], "%s,\n", boolean);
    free(boolean);
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: print an array.
  }
  else if (node->type == JSON_OBJECT)
  {
    sprintf(&buffer[position], "{\n");
  }
  else
  {
    return JSON_ERROR;
  }

  // Print the buffer as it is right now.
  if (file == NULL)
  {
    // Print it to stdin.
    printf("%s", buffer);
  }
  else
  {
    // Print it to the file.
    fprintf(file, "%s", buffer);
  }

  // Decide depending on the type of node.
  if (node->type == JSON_OBJECT)
  {
    // Call this function recursively on its childs.
    for (int i = 0; i < ((OBJDATA *) node->data)->childNumber; i++)
    {
      printNode(file, ((OBJDATA *) node->data)->objectPointers[i], offset + 1);
    }

    // Finish writing an object node.
    memset(buffer, '\0', bufferSize);
    written = printBlanks(buffer, offset);
    sprintf(&buffer[written], "},\n");

    if (file == NULL)
    {
      printf("%s", buffer);
    }
    else
    {
      fprintf(file, "%s", buffer);
    }
  }

  // If the node is not an object, we have already finished.
  return JSON_OK;
}


// Writes blank spaces to a string.
// Returns the number of characters written.
static int printBlanks(char *string, int number)
{
  memset(string, ' ', 2 * number);
  return 2 * number;
}
