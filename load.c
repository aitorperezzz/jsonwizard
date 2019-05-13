// This module receives a .json file, imports the information to a node
// structure and returns it.

// Include header files.
#include "load.h"

// Static variables.
static int INITIAL_STRING_LENGTH = 100;

// Prototypes for static functions.
static char *fileToString(FILE *);
static NODE *jsonParse(char *);
static NODE *parseNode(char *, int);
static int stepsToChar(char *, int, char);
static int sizeOfNumber(char *, int);
static int getSizeOfNextNode(char *, int);
static int getSizeOfObjectValue(char *, int);

// Receives a filename and returns a pointer to a JSON object if succesful.
// If not succcessful, returns NULL.
NODE *jsonLoad(char *filename)
{
  // Try to open the specified file.
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("ERROR. Could not open the specified file.\n");
    return NULL;
  }

  // Try to convert it to a string.
  char *jsonString = fileToString(file);
  if (jsonString == NULL)
  {
    printf("ERROR. Could not parse the file to a string.\n");
    return NULL;
  }

  printf("%s\n", jsonString);
  printf("Length of the JSON string: %ld.\n", strlen(jsonString));

  // Now we have a string with the JSON content. We try to parse it.
  NODE *root = jsonParse(jsonString);
  if (root == NULL)
  {
    printf("ERROR: The JSON string could not be parsed.\n");
    free(jsonString);
    return NULL;
  }

  free(jsonString);
  return root;
}


// Receives a file pointer and parses it to a string,
// removing spaces and newline characters.
// Returns a pointer to the string if successful.
// Returns NULL if not successful.
static char *fileToString(FILE *file)
{
  // Create a buffer for the string.
  char *string = calloc(INITIAL_STRING_LENGTH, sizeof(char));
  int length = INITIAL_STRING_LENGTH;
  int position = 0;

  char character;
  while (1)
  {
    character = fgetc(file);
    if (character != EOF)
    {
      if (character != '\n' && character != ' ')
      {
        // Only in this case, copy it to the string.
        string[position] = character;
        position++;
      }
    }

    // Check if we have space in the string buffer.
    if (position == length)
    {
      // Reserve more space.
      string = realloc(string, length + INITIAL_STRING_LENGTH);
      length = length + INITIAL_STRING_LENGTH;
      if (string == NULL)
      {
        printf("ERROR. Could not read from file, not enough memory.\n");
        free(string);
        return NULL;
      }
    }

    if (character == EOF)
    {
      break;
    }
  }

  // Close the string and return.
  string[position] = '\0';
  return string;
}


// Receives a JSON string and tries to parse it to a structure.
// Returns a pointer to the structure if successful.
// Returns NULL if not.
static NODE *jsonParse(char *string)
{
  // Add "root": to the string.
  char *rootString = "\"root\":";
  int size = strlen(rootString) + strlen(string) + 1;
  char nodeString[size];
  memcpy(nodeString, rootString, strlen(rootString));
  memcpy(&nodeString[strlen(rootString)], string, strlen(string));
  nodeString[size - 1] = '\0';

  // Parse the root node.
  NODE *root = parseNode(nodeString, size - 1);
  if (root == NULL)
  {
    printf("ERROR. Could not parse the root node.\n");
    return NULL;
  }

  return root;
}


// Receives a pointer to a buffer where a node is represented,
// and the length of that node.
// Returns a node pointer if successful.
// Returns NULL if not.
static NODE *parseNode(char *string, int length)
{
  int position;

  // The first thing in the node is the key.
  int keyLength = stepsToChar(&string[1], length - 1, '\"');
  if (keyLength == -1)
  {
    printf("ERROR: Could not find the key in the node.\n");
    return NULL;
  }

  // Store the key; keyLength is the length of the key.
  char key[STRING_LENGTH + 1];
  memcpy(key, string + 1, keyLength);
  key[keyLength] = '\0';

  // Get the position of the value in the node.
  position = keyLength + 3;

  // Create some space for the node.
  NODE *node = malloc(sizeof(NODE));
  initializeNode(node);
  // TODO: wrap this with abstraction.
  setKey(node, key);

  // Access the character in position to get the type.
  if (string[position] == 'n')
  {
    // This is a null node. Nothing to do.
  }
  else if (string[position] == '\"')
  {
    // This is a string node. Find the string.
    int valueLength = stepsToChar(&string[position + 1], length - position - 1, '\"');
    if (valueLength == -1)
    {
      printf("ERROR: Could not find value in STRING node.\n");
      return NULL;
    }
    char value[STRING_LENGTH + 1];
    memcpy(value, &string[position + 1], valueLength);
    value[valueLength] = '\0';

    // Update node information.
    setType(node, JSON_STRING);
    setData(node, value);
  }
  else if (isdigit(string[position]))
  {
    // This is an integer node. First get the value of the int.
    int numberLength = sizeOfNumber(&string[position], length - position);
    char numberString[STRING_LENGTH + 1];
    memcpy(numberString, &string[position], numberLength);
    numberString[numberLength] = '\0';

    // Update the information of the node.
    setType(node, JSON_INTEGER);
    setData(node, numberString);
  }
  else if (string[position] == 't' || string[position] == 'f')
  {
    // This is a boolean node.
    setType(node, JSON_BOOLEAN);

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
    return NULL;
  }
  else if (string[position] == '{')
  {
    // This is an object node. Set it as an object node.
    setType(node, JSON_OBJECT);
    int pointer = position + 1;

    // Parse the object childs looking for nodes.
    OBJDATA *data = (OBJDATA *) node->data;
    NODE *newNode;
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
          printf("ERROR: Could not create node beginning at position %d.\n", pointer);
          printf("ERROR: String received by parseNode: %s.\n", &string[pointer]);
          return NULL;
        }

        // Add the new node to the child array.
        newNode->parent = node;
        data->objectPointers[data->childNumber] = newNode;
        data->childNumber++;

        // Prepare for the next node.
        pointer = pointer + size + 1;
      }
    }
  }
  else
  {
    // Error parsing the node.
    return NULL;
  }

  // The node has been parsed, so return it.
  return node;
}

// Receives a pointer to a buffer of chars and the length of the buffer.
// Receives a char and returns the number of chars until that char is found.
static int stepsToChar(char *string, int length, char c)
{
  int position = 0;
  while (position < length && string[position] != c)
  {
    position++;
  }

  if (position == length)
  {
    // It could not be found.
    return -1;
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
// the end of the next complete node.
// Returns -1 if anything went wrong.
static int getSizeOfNextNode(char *string, int length)
{
  // Find the length of the next key.
  int valuePosition = stepsToChar(string, length, ':') + 1;
  int size;

  if (valuePosition >= length)
  {
    return -1;
  }

  // Check the next character.
  if (string[valuePosition] == '{')
  {
    size = getSizeOfObjectValue(&string[valuePosition], length - valuePosition);
  }
  else
  {
    // Just look for the next comma or for the end of the buffer.
    int position = valuePosition;
    while (position < length && string[position] != ',' && string[position] != '}')
    {
      position++;
    }
    size = position - valuePosition;
    //size = stepsToChar(&string[steps + 1], length - steps - 1, ',');
  }

  // Add the : char and the , char and you get the size of the node.
  return valuePosition + size;
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
