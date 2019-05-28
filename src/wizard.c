// Includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wizard.h"
#include "print.h"
#include "load.h"
#include "set.h"
#include "free.h"

// Main.
int main(int argc, char **argv)
{
  // Variable to store the input from the user.
  int inputLength = (STRING_LENGTH + 1) * COMMAND_WORDS;
  char input[inputLength];

  // Variable to store the number of words in the current command.
  int count = 0;

  // Variables to store the words in the command.
  char *words[COMMAND_WORDS];
  for (int i = 0; i < COMMAND_WORDS; i++)
  {
    words[i] = malloc(STRING_LENGTH + 1);
    strcpy(words[i], "");
  }

  // Create the root node of the tree.
  node_t *root = createRoot();
  if (root == NULL)
  {
    return -1;
  }

  // Store the result of every operation.
  int result;

  printHelp();

  // Listen to commands in a loop.
  while (1)
  {
    resetCommand(&words[0], &count);
    printf("\nJSONwizard> ");
    if (fgets(input, inputLength, stdin) == NULL)
    {
      printf("ERROR: there was an error reading the command. Continuing...\n");
      continue;
    }

    // Parse the input into words.
    if (parseCommand(input, &words[0], &count) == JSON_ERROR)
    {
      printf("ERROR: there was an error parsing the command. Continuing...\n");
      continue;
    }

    // Execute the command and treat the errors.
    result = executeCommand(words, count, &root);
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
      printf("Exiting the program...\n");
      break;
    }
  }
}


// Prints the help menu.
int printHelp(void)
{
  printf("How to use JSONwizard:\n");
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


// Reset all words in the command to empty words.
int resetCommand(char **words, int *count)
{
  *count = 0;
  for (int i = 0; i < COMMAND_WORDS; i++)
  {
    strcpy(words[i], "");
  }
  return JSON_OK;
}


// Parse the command into its words.
int parseCommand(char *input, char **words, int *count)
{
  // Create a buffer for the current word.
  char buffer[STRING_LENGTH + 1];

  // Counter for letters.
  int letterCount = 0;

  // Counter for total words.
  int wordCount = 0;

  for (int i = 0, n = strlen(input); i < n; i++)
  {
    if (input[i] == ' ')
    {
      // Reached the end of a word.
      buffer[letterCount] = '\0';
      strcpy(words[wordCount], buffer);
      strcpy(buffer, "");
      wordCount++;
      letterCount = 0;
      continue;
    }
    if (i == n - 1)
    {
      // This last character is the new line.
      buffer[letterCount] = '\0';
      strcpy(words[wordCount], buffer);
      strcpy(buffer, "");
      wordCount++;
      break;
    }
    buffer[letterCount] = input[i];
    letterCount++;
  }

  // Set the word count.
  *count = wordCount;
  return JSON_OK;
}


// Execute the command.
int executeCommand(char **command, int count, node_t **rootAddress)
{
  if (strcmp(command[0], "append") == 0)
  {
    // Structure: append <child key> to <parent key>.
    return jsonAppend(rootAddress, command[3], command[1]);
  }
  else if (strcmp(command[0], "modify") == 0)
  {
    // Structure: modify <key> set <field> <value>.
    return jsonModify(*rootAddress, command[1], command[3], command[4]);
  }
  else if (strcmp(command[0], "delete") == 0)
  {
    // Structure: delete [<key>].
    return jsonDelete(rootAddress, command[1]);
  }
  else if (strcmp(command[0], "write") == 0)
  {
    // Structure: write <filename>
    return jsonPrintToFile(*rootAddress, command[1]);
  }
  else if (strcmp(command[0], "print") == 0)
  {
    // Structure: print [<key>].
    return jsonPrintToStdin(*rootAddress, command[1]);
  }
  else if (strcmp(command[0], "load") == 0)
  {
    // Structure: load <filename>.
    return jsonLoad(rootAddress, command[1]);
  }
  else if (strcmp(command[0], "quit") == 0)
  {
    return jsonQuit(*rootAddress, command);
  }
  else if (strcmp(command[0], "help") == 0)
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
node_t *createNode(void)
{
  // Try to create space for a new node.
  node_t *node = malloc(sizeof(node_t));
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
node_t *createRoot(void)
{
  node_t *root = createNode();
  if (root == NULL)
  {
    printf("ERROR: could not create the root node.\n");
    return NULL;
  }

  setKey(root, "root");
  setType(root, TYPE_OBJECT);
  return root;
}


// Initialize a node to the default values.
int initializeNode(node_t *node)
{
  if (node == NULL)
  {
    printf("ERROR: cannot initialize a NULL node.\n");
    return JSON_ERROR;
  }

  node->type = TYPE_NULL;
  strcpy(node->key, "");
  node->parent = NULL;
  node->data = NULL;

  return JSON_OK;
}


// Receives a type code and validates it.
int validateType(int type)
{
  switch (type)
  {
    case TYPE_NULL:
    case TYPE_STRING:
    case TYPE_INTEGER:
    case TYPE_BOOLEAN:
    case TYPE_ARRAY:
    case TYPE_OBJECT:
      return JSON_OK;
    default:
      return JSON_ERROR;
  }
}

// Receives a field code and validates it.
int validateField(int field)
{
  switch (field)
  {
    case FIELD_TYPE:
    case FIELD_KEY:
    case FIELD_PARENT:
    case FIELD_DATA:
      return JSON_OK;
    default:
      return JSON_ERROR;
  }
}


// Gets the boolean as a string and returns its code.
int booleanStringToCode(char *string)
{
  if (strcmp(string, "true") == 0)
  {
    return BOOL_TRUE;
  }
  else if (strcmp(string, "false") == 0)
  {
    return BOOL_FALSE;
  }
  else
  {
    printf("ERROR. %s is not a valid boolean type.\n", string);
    return BOOL_UNKNOWN;
  }
}

// Gets a boolean code and converts it to a string
// storing it in the buffer provided.
int booleanCodeToString(int code, char *buffer)
{
  switch (code)
  {
    case BOOL_TRUE:
      strcpy(buffer, "true");
      return JSON_OK;
    case BOOL_FALSE:
      strcpy(buffer, "false");
      return JSON_OK;
    default:
      printf("ERROR: boolean code is not valid.\n");
      return JSON_ERROR;
  }
}


// Receives a string with the type of node.
// and tries to convert to the type code.
int typeStringToCode(char *string)
{
  if (string == NULL)
  {
    printf("ERROR: could not get the code for the type. No type provided.\n");
    return TYPE_NULL;
  }

  // Decide depending on the string.
  if (strcmp(string, "null") == 0)
  {
    return TYPE_NULL;
  }
  else if (strcmp(string, "string") == 0)
  {
    return TYPE_STRING;
  }
  else if (strcmp(string, "integer") == 0)
  {
    return TYPE_INTEGER;
  }
  else if (strcmp(string, "boolean") == 0)
  {
    return TYPE_BOOLEAN;
  }
  else if (strcmp(string, "array") == 0)
  {
    return TYPE_ARRAY;
  }
  else if (strcmp(string, "object") == 0)
  {
    return TYPE_OBJECT;
  }
  else
  {
    printf("ERROR: could not convert string '%s' to a valid type.\n", string);
    return TYPE_NULL;
  }
}


// Append a node to a parent node.
int jsonAppend(node_t **rootAddress, char *parentKey, char *childKey)
{
  node_t *root = *rootAddress;

  if (root == NULL)
  {
    printf("ERROR: cannot append node because root is NULL.\n");
    return JSON_ERROR;
  }

  // Try to find the parent node.
  node_t *parent = searchByKey(root, parentKey);
  if (parent == NULL)
  {
    printf("ERROR: could not find the parent key in the root node.\n");
    return JSON_ERROR;
  }
  else if (validateType(parent->type) == JSON_ERROR)
  {
    printf("ERROR: cannot append node to a node without a valid type.\n");
    return JSON_ERROR;
  }
  else if (parent->type != TYPE_OBJECT)
  {
    printf("ERROR: cannot append node to a node that is not of type object.\n");
    return JSON_ERROR;
  }

  // TODO: prevent from appending when the key is already found.

  // Only in this case create a new node.
  node_t *newNode = createNode();
  if (newNode == NULL)
  {
    printf("ERROR: could not create a child node.\n");
    return JSON_ERROR;
  }
  setKey(newNode, childKey);

  // Link the parent to the child.
  int childs = ((objData_t *) parent->data)->childNumber;
  ((objData_t *) parent->data)->objects[childs] = newNode;
  setChildNumber(parent, childs + 1);
  newNode->parent = parent;

  return JSON_OK;
}


// Tries to find, recursively, a node with the specified key.
// If found, returns a pointer to that node.
node_t *searchByKey(node_t *node, char *key)
{
  if (node == NULL)
  {
    printf("ERROR: cannot find key in NULL node.\n");
    return NULL;
  }

  if (strcmp(node->key, key) == 0)
  {
    // This node has the key.
    return node;
  }
  else if (node->type == TYPE_OBJECT)
  {
    // Look recursively in this case.
    node_t *result;
    objData_t *data = (objData_t *)node->data;
    for (int i = 0; i < data->childNumber; i++)
    {
      if (data->objects[i] != NULL)
      {
        result = searchByKey(data->objects[i], key);
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
