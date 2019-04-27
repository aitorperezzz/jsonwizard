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
  JSON_QUIT
} resultCode;


// STRUCTURES.
// An enum for the type of node.
typedef enum nodeType_enum
{
  JSON_DEFAULT, /* Node is not yet initialized */
  JSON_PAIR,
  JSON_OBJECT
} nodeType;

// Node structure.
struct node_st
{
  nodeType type;
  char key[STRING_LENGTH + 1];
  int childNumber;
  void *data;
  struct node_st *parent;
};
typedef struct node_st node;


// PROTOTYPES.
int resetCommand(char **, int *);
int parseCommand(char *, char **, int *);
int printCommand(char **, int);
int executeCommand(char **, int, node **);
int printHelp(void);

int setType(node *, int);
int setKey(node *, char *);
int setChildNumber(node *, int);
int initializeData(node *);
int initializeNode(node *);

node *searchByKey(node *, char *);

int jsonAppend(node **, char *, char *);

int jsonModify(struct node_st *, char *, char *, char *);
int typeStringToCode(char *);
int setData(struct node_st *, char *);

int jsonDelete(struct node_st **, char *);
int jsonFreeNode(struct node_st *);

int jsonPrint(node *, char *);
int printBlanks(int);
int printNode(node *, int);

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

  // Create the root tree.
  struct node_st *root = malloc(sizeof(struct node_st));
  initializeNode(root);
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

int printHelp(void)
{
  printf("How to use JSONwizard:\n");
  printf("To add a node to another node:\n");
  printf("\t> append <child key> to <parent key>\n");
  printf("To modify some values inside a node:\n");
  printf("\t> modify <key> set <field> <value>\n");
  printf("To delete a node and all of its childs.\n");
  printf("\t> delete [<key>]\n");
  printf("To print information about the tree:\n");
  printf("\t> print [<key>]\n");
  printf("To print this help menu:\n");
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
int executeCommand(char **command, int count, node **rootAddress)
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


// Initialize a node to the default values.
int initializeNode(struct node_st *node)
{
  if (node == NULL)
  {
    printf("Cannot initialize a NULL node.\n");
    return JSON_ERROR;
  }

  setType(node, JSON_DEFAULT);
  setKey(node, "");
  setChildNumber(node, 0);
  node->data = NULL;
  node->parent = NULL;

  return JSON_OK;
}


// Set the type of a node.
int setType(node *node, int type)
{
  // Check if the node is available.
  if (node == NULL)
  {
    printf("ERROR: Cannot set type of NULL node.\n");
    return JSON_ERROR;
  }

  if (!(type == JSON_DEFAULT || type == JSON_PAIR || type == JSON_OBJECT))
  {
    printf("ERROR: Cannot set type. Not a valid type.\n");
    return JSON_ERROR;
  }
  else
  {
    node->type = type;
  }

  // Initialize data only in case of pair and object.
  if (type == JSON_PAIR || type == JSON_OBJECT)
  {
    return initializeData(node);
  }

  return JSON_OK;
}


// Set the key of a node.
int setKey(node *node, char *key)
{
  if (node != NULL)
  {
    strcpy(node->key, key);
    return JSON_OK;
  }

  return JSON_ERROR;
}


// Set the number of childs in a node.
int setChildNumber(node *node, int number)
{
  if (node != NULL)
  {
    node->childNumber = number;
    return JSON_OK;
  }

  return JSON_ERROR;
}


// Set the data in a node.
int setData(struct node_st *node, char *data)
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
  if (node->type == JSON_DEFAULT)
  {
    printf("ERROR. Cannot modify data in a node with type DEFAULT.\n");
    return JSON_ERROR;
  }
  else if (node->type == JSON_OBJECT)
  {
    printf("ERROR. To modify data in a node of type OBJECT, please use append.\n");
    return JSON_ERROR;
  }
  else if (node->type == JSON_PAIR)
  {
    strcpy(node->data, data);
    return JSON_OK;
  }

  return JSON_ERROR;
}


// Initializes the data of a node.
int initializeData(node *thisNode)
{
  // Check if there is a node.
  if (thisNode ==  NULL)
  {
    printf("ERROR: Cannot initialize data of NULL node.\n");
    return JSON_ERROR;
  }

  // Check the type of node.
  if (thisNode->type == JSON_DEFAULT)
  {
    printf("ERROR: Cannot initialize data in a node of type default.\n");
    return JSON_ERROR;
  }
  else if (thisNode->type == JSON_PAIR)
  {
    // Create space for a string and initialize it to blank.
    thisNode->data = malloc(STRING_LENGTH + 1);
    strcpy((char *) thisNode->data, "");
    return JSON_OK;
  }
  else if (thisNode->type == JSON_OBJECT)
  {
    // Create an array of node pointers. Point each one to NULL.
    thisNode->data = malloc(NUMBER_OBJECTS * sizeof(node *));
    for (int i = 0; i < NUMBER_OBJECTS; i++)
    {
      ((node **) thisNode->data)[i] = NULL;
    }
    return JSON_OK;
  }

  return JSON_ERROR;
}


// Append a node to a parent node.
int jsonAppend(node **rootAddress, char *parentKey, char *childKey)
{
  node *root = *rootAddress;

  if (root == NULL)
  {
    printf("Cannot append to a NULL root.\n");
    return JSON_ERROR;
  }

  // Find the parent node.
  node *parent = searchByKey(root, parentKey);

  // Check if parent is a nice node.
  if (parent == NULL)
  {
    printf("ERROR: Could not find the parent key in the tree.\n");
    return JSON_ERROR;
  }
  if (parent->type == JSON_DEFAULT)
  {
    printf("ERROR: Cannot append node to node of type default.\n");
    return JSON_ERROR;
  }
  if (parent->type == JSON_PAIR)
  {
    printf("Cannot append node to node of type pair.\n");
    return JSON_ERROR;
  }

  // Create a new node.
  struct node_st *newNode = malloc(sizeof(struct node_st));
  initializeNode(newNode);
  setKey(newNode, childKey);

  // Link the parent to the new node.
  ((struct node_st **) parent->data)[parent->childNumber] = newNode;
  setChildNumber(parent, parent->childNumber + 1);
  newNode->parent = parent;

  return JSON_OK;
}


// Find the node with the given key.
struct node_st *searchByKey(node *currentNode, char *key)
{
  if (currentNode == NULL)
  {
    printf("ERROR: Cannot find key in NULL node.\n");
    return NULL;
  }

  if (strcmp(currentNode->key, key) == 0)
  {
    // This node has the key.
    return currentNode;
  }
  else if (currentNode->type == JSON_OBJECT)
  {
    // Only in this case look recursively.
    node *result;
    for (int i = 0; i < currentNode->childNumber; i++)
    {
      if (((node **) currentNode->data)[i] != NULL)
      {
        result = searchByKey(((node **) currentNode->data)[i], key);
        if (result != NULL)
        {
          return result;
        }
      }
    }
  }
  else
  {
    // This node does not have the key and is not object.
    return NULL;
  }
}


// This function modifies the node indicated by its key.
int jsonModify(struct node_st *root, char *key, char *field, char *value)
{
  printf("length of value: %ld\n", strlen(value));
  // Check root is available.
  if (root == NULL)
  {
    printf("ERROR: jsonModify. .\n");
    return JSON_ERROR;
  }

  // Try to find the relevant node.
  struct node_st *node = searchByKey(root, key);
  if (node == NULL)
  {
    printf("ERROR: Cannot modify node. Key was not found.\n");
    return JSON_ERROR;
  }

  // Decide based on the field.
  if (strcmp(field, "type") == 0)
  {
    return setType(node, typeStringToCode(value));
  }
  else if (strcmp(field, "key") == 0)
  {
    return setKey(node, key);
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


// Reecives a string with the type of node
// and tries to convert to the type code.
int typeStringToCode(char *type)
{
  if (strcmp(type, "pair") == 0)
  {
    return JSON_PAIR;
  }
  else if (strcmp(type, "object") == 0)
  {
    return JSON_OBJECT;
  }
  else
  {
    return JSON_DEFAULT;
  }
}


// Deletes the optionally selected node from the tree.
// If no key is given, the whole tree is deleted.
int jsonDelete(struct node_st **rootAddress, char *what)
{
  // Check the root tree is available.
  if (*rootAddress == NULL)
  {
    printf("ERROR: Cannot delete node. Root tree is NULL.\n");
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
  struct node_st *node = searchByKey(*rootAddress, key);

  if (node == NULL)
  {
    printf("ERROR: Cannot delete node. Key was not found.\n");
    return JSON_ERROR;
  }

  // Tre to get the parent node.
  struct node_st *parent = node->parent;

  if (parent == NULL)
  {
    if (strcmp(node->key, "root") == 0)
    {
      // The user wants to delete the root tree.
      jsonFreeNode(*rootAddress);
      *rootAddress = malloc(sizeof(struct node_st));
      initializeNode(*rootAddress);
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
  ((struct node_st **) parent->data)[parent->childNumber] = NULL;
  setChildNumber(parent, parent->childNumber - 1);

  return JSON_OK;
}


// Receives a pointer to a node and frees it.
int jsonFreeNode(struct node_st *node)
{
  // Check that the node is available.
  if (node == NULL)
  {
    printf("ERROR: Cannot free NULL node.\n");
    return JSON_ERROR;
  }

  // Decide according to the type of node.
  if (node->type == JSON_PAIR || node->type == JSON_DEFAULT)
  {
    free(node);
    return JSON_OK;
  }
  else if (node->type == JSON_OBJECT)
  {
    // Loop through all of its children nodes.
    for (int i = 0; i < node->childNumber; i++)
    {
      jsonFreeNode(((struct node_st **) node->data)[i]);
    }
    free(node);
    return JSON_OK;
  }
}


// Function that handles the print command.
int jsonPrint(node *root, char *what)
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

  node *parent = searchByKey(root, key);

  if (parent != NULL)
  {
    printNode(parent, 0);
  }
}


// Print the selected node and childs with the given offset on screen.
int printNode(struct node_st *node, int offset)
{
  // Print the offset.
  printBlanks(offset);

  // Print the key.
  printf("\"%s\" : ", node->key);

  if (node->type == JSON_PAIR)
  {
    // Only print the string.
    printf("\"%s\"\n", (char *) node->data);
  }
  else if (node->type == JSON_OBJECT)
  {
    // Print a brace and call printNode in a loop.
    printf("{\n");
    for (int i = 0; i < node->childNumber; i++)
    {
      printNode(((struct node_st **) node->data)[i], offset + 1);
    }
    printBlanks(offset);
    printf("}\n");
  }
  else if (node->type == JSON_DEFAULT)
  {
    // This node does not have information.
    printf("<empty>\n");
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
