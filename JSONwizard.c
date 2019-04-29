// INCLUDES.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// DEFINES.
// Lengths of variables.
#define STRING_LENGTH 100
#define NUMBER_WORDS 20
#define NUMBER_OBJECTS 20

// Result codes.
typedef enum resultCode_enum
{
  JSON_OK,
  JSON_ERROR,
  JSON_QUIT,
} RESULTCODE;


// STRUCTURES.
// Enum for the type of node.
typedef enum nodeType_enum
{
  JSON_NULL, /* Node has not a valid type yet. */
  JSON_STRING,
  JSON_INTEGER,
  JSON_BOOLEAN,
  JSON_ARRAY,
  JSON_OBJECT
} NODETYPE;

// Enum for the boolean codes.
typedef enum boolean_enum
{
  JSON_TRUE,
  JSON_FALSE,
  JSON_UNKNOWN
} BOOLEAN;

// Node structure.
struct node_st
{
  NODETYPE type;
  char key[STRING_LENGTH + 1];
  struct node_st *parent;
  void *data;
};
typedef struct node_st NODE;

// Data pointed to when the node is of type object.
struct object_data
{
  int childNumber;
  NODE *objectPointers[NUMBER_OBJECTS];
};
typedef struct object_data OBJDATA;

// TODO: create a structure for the array data.


// PROTOTYPES.
int resetCommand(char **, int *);
int parseCommand(char *, char **, int *);
int printCommand(char **, int);
int executeCommand(char **, int, NODE **);
int printHelp(void);

NODE *createNode(void);
int setType(NODE *, int);
int validateType(int type);
int setKey(NODE *, char *);
int setChildNumber(NODE *, int);
int initializeData(NODE *);
int initializeNode(NODE *);

NODE *searchByKey(NODE *, char *);

int jsonAppend(NODE **, char *, char *);

int jsonModify(NODE *, char *, char *, char *);
int typeStringToCode(char *);
int setData(NODE *, char *);
int getBooleanFromString(char *data);
int initializeObjectData(OBJDATA *objData);

int jsonDelete(NODE **, char *);
int jsonFreeNode(NODE *);

int jsonPrint(NODE *, char *);
int printBlanks(int);
int printNode(NODE *, int);

// MAIN.
int main(int argc, char argv[])
{
  // Variable to store the input from the user.
  int inputLength = (STRING_LENGTH + 1) * NUMBER_WORDS + 1;
  char input[inputLength];

  // Variable to store the number of words in the current command.
  int count = 0;

  // Variables to store the command words.
  char *words[NUMBER_WORDS];
  for (int i = 0; i < NUMBER_WORDS; i++)
  {
    words[i] = malloc(STRING_LENGTH + 1);
    strcpy(words[i], "");
  }

  // Create the root node of the tree.
  NODE *root = createNode();
  setKey(root, "root");
  setType(root, JSON_OBJECT);

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
      printf("There was an error reading the command. Continuing\n");
      continue;
    }

    // Parse the input into words.
    if (parseCommand(input, &words[0], &count) != JSON_OK)
    {
      printf("There was an error parsing the command. Continuing\n");
      continue;
    }

    // Print the command to be executed.
    printCommand(&words[0], count);

    // Execute the command and treat the errors.
    result = executeCommand(words, count, &root);
    if (result == JSON_OK)
    {
      printf("The command executed correctly. Continuing.\n");
      continue;
    }
    else if (result == JSON_ERROR)
    {
      printf("The command could not be executed. Continuing.\n");
      continue;
    }
    else if (result == JSON_QUIT)
    {
      printf("Exiting the program.\n");
      break;
    }
  }

  // Write to an external file.
  // TODO.

  // Clean the word space.
  for (int i = 0; i < NUMBER_WORDS; i++)
  {
    free(words[i]);
  }
}


// Prints the help menu.
int printHelp(void)
{
  printf("How to use JSONwizard:\n");
  printf("\t> append <child key> to <parent key>\n");
  printf("\t> modify <key> set <field> <value>\n");
  printf("\t> delete [<key>]\n");
  printf("\t> print [<key>]\n");
  printf("\t> help\n");

  return JSON_OK;
}


// Reset all words in the command to empty words.
int resetCommand(char **words, int *count)
{
  *count = 0;
  for (int i = 0; i < NUMBER_WORDS; i++)
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


// Prints the command to be executed, once parsed.
int printCommand(char **words, int count)
{
  printf("Command to be executed: ");
  for (int i = 0; i < count; i++)
  {
    printf("%s ", words[i]);
  }
  printf("\n");
  return JSON_OK;
}


// Execute the command.
int executeCommand(char **command, int count, NODE **rootAddress)
{
  if (strcmp(command[0], "append") == 0)
  {
    // Strcuture: append <child key> to <parent key>.
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
  else if (strcmp(command[0], "print") == 0)
  {
    // Structure: print [<what>].
    return jsonPrint(*rootAddress, command[1]);
  }
  else if (strcmp(command[0], "help") == 0)
  {
    return printHelp();
  }
  else
  {
    printf("The command could not be understood.\n");
    return JSON_ERROR;
  }
}

// Tries to create a node and returns a pointer to that node.
NODE *createNode(void)
{
  NODE *node = malloc(sizeof(NODE));
  if (node != NULL)
  {
    if (initializeNode(node) == JSON_OK)
    {
      return node;
    }
  }

  printf("ERROR: Could not allocate memory for the node.\n");
  return NULL;
}


// Initialize a node to the default values.
int initializeNode(NODE *node)
{
  if (node == NULL)
  {
    printf("ERROR: Cannot initialize a NULL node.\n");
    return JSON_ERROR;
  }

  setType(node, JSON_NULL);
  setKey(node, "");
  node->parent = NULL;
  node->data = NULL;

  return JSON_OK;
}


// Set the type of a node.
int setType(NODE *node, int type)
{
  // Check if the node is available.
  if (node == NULL)
  {
    printf("ERROR: Cannot set type of NULL node.\n");
    return JSON_ERROR;
  }

  if (validateType(type) == JSON_ERROR)
  {
    printf("ERROR: Cannot set type. Not a valid type.\n");
    return JSON_ERROR;
  }
  else
  {
    node->type = type;
  }

  // Initialize the data if possible.
  switch (type)
  {
    case JSON_STRING:
    case JSON_INTEGER:
    case JSON_BOOLEAN:
    case JSON_ARRAY:
    case JSON_OBJECT:
      return initializeData(node);
    case JSON_NULL:
      return JSON_OK;
  }
}


// Receives a type and checks if it is valid.
int validateType(int type)
{
  switch (type)
  {
    case JSON_NULL:
    case JSON_STRING:
    case JSON_INTEGER:
    case JSON_BOOLEAN:
    case JSON_ARRAY:
    case JSON_OBJECT:
      return JSON_OK;
    default:
      return JSON_ERROR;
  }
}


// Set the key of a node.
int setKey(NODE *node, char *key)
{
  if (node == NULL)
  {
    printf("ERROR: Cannot set key of NULL node.\n");
    return JSON_ERROR;
  }

  strcpy(node->key, key);
  return JSON_OK;
}


// Set the number of childs in a node of type object.
int setChildNumber(NODE *node, int number)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: Cannot change child number in NULL node.\n");
    return JSON_ERROR;
  }

  if (node->type != JSON_OBJECT)
  {
    printf("ERROR. Cannot change child number. Node is not object type.\n");
    return JSON_ERROR;
  }

  ((OBJDATA *) node->data)->childNumber = number;
  return JSON_OK;
}


// Set the data in a node.
int setData(NODE *node, char *data)
{
  // Check the node is accesible.
  if (node == NULL)
  {
    printf("ERROR: Cannot modify data in NULL node.\n");
    return JSON_ERROR;
  }

  // Check data is accesible.
  if (data == NULL)
  {
    printf("ERROR. Cannot modify data. No data was provided.\n");
    return JSON_ERROR;
  }

  // Decide according to the type of node.
  if (node->type == JSON_NULL)
  {
    printf("ERROR. Cannot modify data in a node with type NULL.\n");
    return JSON_ERROR;
  }
  else if (node->type == JSON_STRING)
  {
    strcpy((char *) node->data, data);
    return JSON_OK;
  }
  else if (node->type == JSON_INTEGER)
  {
    *((int *) node->data) = atoi(data);
    return JSON_OK;
  }
  else if (node->type == JSON_BOOLEAN)
  {
    int boolean = getBooleanFromString(data);
    if (boolean == JSON_TRUE || boolean == JSON_FALSE)
    {
      *((int *) node->data) = boolean;
      return JSON_OK;
    }
    else
    {
      printf("ERROR: Not a valid boolean type");
      return JSON_ERROR;
    }
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: parse the string with array values and load to data.
    printf("To modify data in a node of type ARRAY, not implemented.\n");
    return JSON_ERROR;
  }
  else if (node->type == JSON_OBJECT)
  {
    printf("ERROR. To modify data in a node of type OBJECT, please use append.\n");
    return JSON_ERROR;
  }

  return JSON_ERROR;
}


// Gets a boolean from a string and returns its integer code.
int getBooleanFromString(char *data)
{
  if (strcmp(data, "true") == 0)
  {
    return JSON_TRUE;
  }
  else if (strcmp(data, "false") == 0)
  {
    return JSON_FALSE;
  }
  else
  {
    printf("ERROR. %s is not a valid boolean type.\n", data);
    return JSON_UNKNOWN;
  }
}


// Initializes the data of a node once its type is known.
int initializeData(NODE *node)
{
  // Check if the node is available.
  if (node ==  NULL)
  {
    printf("ERROR: Cannot initialize data pointer of NULL node.\n");
    return JSON_ERROR;
  }

  // Decide depending on the type of node.
  if (node->type == JSON_NULL)
  {
    printf("ERROR: Cannot initialize data in a node of type NULL.\n");
    return JSON_ERROR;
  }
  else if (node->type == JSON_STRING)
  {
    // Create space for a string and initialize it to empty.
    node->data = malloc(STRING_LENGTH + 1);
    strcpy((char *) node->data, "");
    return JSON_OK;
  }
  else if (node->type == JSON_INTEGER)
  {
    // Create space for an integer and store a zero.
    node->data = malloc(sizeof(int));
    *((int *) node->data) = 0;
    return JSON_OK;
  }
  else if (node->type == JSON_BOOLEAN)
  {
    node->data = malloc(sizeof(BOOLEAN));
    *((BOOLEAN *) node->data) = JSON_UNKNOWN;
    return JSON_OK;
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: initialize the array.
    return JSON_ERROR;
  }
  else if (node->type == JSON_OBJECT)
  {
    // Initialize an object data structure.
    node->data = malloc(sizeof(OBJDATA));
    initializeObjectData((OBJDATA *) node->data);
    return JSON_OK;
  }

  return JSON_ERROR;
}


// Initializes an OBJDATA structure.
int initializeObjectData(OBJDATA *objData)
{
  // Check if the pointer is available.
  if (objData == NULL)
  {
    printf("ERROR. Cannot initialize NULL object data pointer.\n");
    return JSON_ERROR;
  }

  objData->childNumber = 0;
  for (int i = 0; i < NUMBER_OBJECTS; i++)
  {
    objData->objectPointers[i] = NULL;
  }

  return JSON_OK;
}


// Append a node to a parent node.
int jsonAppend(NODE **rootAddress, char *parentKey, char *childKey)
{
  NODE *root = *rootAddress;

  if (root == NULL)
  {
    printf("ERROR: Cannot append if root is NULL.\n");
    return JSON_ERROR;
  }

  // Find the parent node.
  NODE *parent = searchByKey(root, parentKey);

  // Check if parent is a good node.
  if (parent == NULL)
  {
    printf("ERROR: Could not find the parent key in the tree.\n");
    return JSON_ERROR;
  }
  else if (validateType(parent->type) == JSON_ERROR)
  {
    printf("ERROR: Cannot append node to a node without a valid type.\n");
    return JSON_ERROR;
  }
  else if (validateType(parent->type) == JSON_ERROR && parent->type != JSON_OBJECT)
  {
    printf("Cannot append node to a node that is not OBJECT type.\n");
    return JSON_ERROR;
  }

  // Create a new node.
  NODE *newNode = createNode();
  setKey(newNode, childKey);

  // Link the parent to the new node.
  int childs = ((OBJDATA *) parent->data)->childNumber;
  ((OBJDATA *) parent->data)->objectPointers[childs] = newNode;
  setChildNumber(parent, childs + 1);
  newNode->parent = parent;

  return JSON_OK;
}


// Find a node with the specified key inside the specified node.
NODE *searchByKey(NODE *node, char *key)
{
  if (node == NULL)
  {
    printf("ERROR: Cannot find key in NULL node.\n");
    return NULL;
  }

  if (strcmp(node->key, key) == 0)
  {
    // This node has the key.
    return node;
  }
  else if (node->type == JSON_OBJECT)
  {
    // Look recursively in this case.
    NODE *result;
    for (int i = 0; i < ((OBJDATA *) node->data)->childNumber; i++)
    {
      if (((OBJDATA *) node->data)->objectPointers[i] != NULL)
      {
        result = searchByKey(((OBJDATA *) node->data)->objectPointers[i], key);
        if (result != NULL)
        {
          return result;
        }
      }
    }
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: implement a search inside the array.
    return NULL;
  }
  else
  {
    // This node does not have the key and is not
    // a searchable node.
    return NULL;
  }
}


// This function modifies the node indicated by its key.
int jsonModify(NODE *root, char *key, char *field, char *value)
{
  // Check root is available.
  if (root == NULL)
  {
    printf("ERROR: Cannot modify node in NULL root.\n");
    return JSON_ERROR;
  }

  // Try to find the relevant node.
  NODE *node = searchByKey(root, key);
  if (node == NULL)
  {
    printf("ERROR: Cannot modify node. Key %s was not found.\n", key);
    return JSON_ERROR;
  }

  // Decide based on the field.
  if (strcmp(field, "type") == 0)
  {
    return setType(node, typeStringToCode(value));
  }
  else if (strcmp(field, "key") == 0)
  {
    return setKey(node, value);
  }
  else if (strcmp(field, "data") == 0)
  {
    return setData(node, value);
  }
  else
  {
    printf("ERROR: Cannot modify node. %s is not a valid field.\n", field);
    return JSON_ERROR;
  }
}


// Receives a string with the type of node.
// and tries to convert to the type code.
int typeStringToCode(char *type)
{
  if (strcmp(type, "string") == 0)
  {
    return JSON_STRING;
  }
  else if (strcmp(type, "integer") == 0)
  {
    return JSON_INTEGER;
  }
  else if (strcmp(type, "boolean") == 0)
  {
    return JSON_BOOLEAN;
  }
  else if (strcmp(type, "array") == 0)
  {
    return JSON_ARRAY;
  }
  else if (strcmp(type, "object") == 0)
  {
    return JSON_OBJECT;
  }
  else
  {
    return JSON_NULL;
  }
}


// Deletes the optionally selected node from the tree.
// If no key is given, the whole tree is deleted.
int jsonDelete(NODE **rootAddress, char *what)
{
  // Check the root tree is available.
  if (*rootAddress == NULL)
  {
    printf("ERROR: Cannot delete node with a NULL root.\n");
    return JSON_ERROR;
  }

  // Decide if the user has given the optional parameter.
  char key[STRING_LENGTH + 1];
  if (strcmp(what, "") == 0)
  {
    // The user wants to delete the root entirely.
    strcpy(key, "root");
  }
  else
  {
    // A key has been provided.
    strcpy(key, what);
  }

  // Try to find the relevant node to delete.
  NODE *node = searchByKey(*rootAddress, key);

  if (node == NULL)
  {
    printf("ERROR: Cannot delete node. Key was not found.\n");
    return JSON_ERROR;
  }

  // Try to get the parent node.
  NODE *parent = node->parent;

  if (parent == NULL)
  {
    if (strcmp(node->key, "root") == 0)
    {
      // The user wants to delete the root tree.
      jsonFreeNode(*rootAddress);
      *rootAddress = createNode();
      //*rootAddress = malloc(sizeof(NODE));
      //initializeNode(*rootAddress);
      setKey(*rootAddress, "root");
      setType(*rootAddress, JSON_OBJECT);
      return JSON_OK;
    }
    else
    {
      printf("ERROR: Cannot delete node. Parent was not found.\n");
      return JSON_ERROR;
    }
  }

  // Free the memory of the node.
  jsonFreeNode(node);

  // Deattach it from its parent.
  int childs = ((OBJDATA *) parent->data)->childNumber;
  ((OBJDATA *) parent->data)->objectPointers[childs] = NULL;
  setChildNumber(parent, childs - 1);

  return JSON_OK;
}


// Receives a pointer to a node and frees it and all its childs.
int jsonFreeNode(NODE *node)
{
  // Check that the node is available.
  if (node == NULL)
  {
    printf("ERROR: Cannot free NULL node.\n");
    return JSON_ERROR;
  }

  // Decide according to the type of node.
  if (node->type != JSON_OBJECT)
  {
    free(node);
    return JSON_OK;
  }
  else
  {
    // Loop through all of its children nodes.
    int childs = ((OBJDATA *) node->data)->childNumber;
    NODE **ptr = ((OBJDATA *) node->data)->objectPointers;
    //NODE *ptr = ((OBJDATA *) node->data)->objectPointers[0];
    for (int i = 0; i < childs; i++)
    {
      jsonFreeNode(ptr[i]);
    }

    free(node);
    return JSON_OK;
  }
}


// Function that handles the print command.
int jsonPrint(NODE *root, char *what)
{
  // Find out the key wanted by the user.
  char key[STRING_LENGTH + 1];
  if (strcmp(what, "") == 0)
  {
    strcpy(key, "root");
  }
  else
  {
    strcpy(key, what);
  }

  // Try to find the node with the relevant key.
  NODE *parent = searchByKey(root, key);
  if (parent != NULL)
  {
    printNode(parent, 0);
  }
}


// Print the selected node and childs with the given offset on screen.
int printNode(NODE *node, int offset)
{
  // Print the offset.
  printBlanks(offset);

  // Print the key.
  printf("\"%s\" : ", node->key);

  if (node->type == JSON_NULL)
  {
    printf("<null>\n");
  }
  else if (node->type == JSON_STRING)
  {
    printf("\"%s\"\n", (char *) node->data);
  }
  else if (node->type == JSON_INTEGER)
  {
    printf("%d\n", *((int *) node->data));
  }
  else if (node->type == JSON_BOOLEAN)
  {
    if (*((int *) node->data) == JSON_TRUE)
    {
      printf("true\n");
    }
    else if (*((int *) node->data) == JSON_FALSE)
    {
      printf("false\n");
    }
    else if (*((int *) node->data) == JSON_UNKNOWN)
    {
      printf("\n");
    }
  }
  else if (node->type == JSON_ARRAY)
  {
    // TODO: print an array.
    return JSON_ERROR;
  }
  else if (node->type == JSON_OBJECT)
  {
    // Print a brace and call printNode in a loop.
    printf("{\n");
    int childs = ((OBJDATA *) node->data)->childNumber;
    NODE **ptr = ((OBJDATA *) node->data)->objectPointers;
    for (int i = 0; i < childs; i++)
    {
      printNode(ptr[i], offset + 1);
    }
    printBlanks(offset);
    printf("}\n");
  }

  return JSON_OK;
}


// Prints that number of blank characters.
int printBlanks(int number)
{
  for (int i = 0; i < number; i++)
  {
    printf("  ");
  }

  return JSON_OK;
}
