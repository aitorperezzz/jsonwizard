// This module receives a .json file, imports the information to a node
// structure and returns it.

// Static variables.
static int INITIAL_STRING_LENGTH = 100;

// Prototypes for static functions.
static char *fileToString(FILE *);
static NODE *parse(char *);

// Receives a filename and returns a pointer to a JSON object if succesful.
// If not succcessful, returns NULL.
NODE *jsonRead(char *filename)
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

  // Now we have a string with the JSON content. We try to parse it.
  NODE *root = parse(jsonString);
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

        // Check if we have space in the string buffer.
        if (position == length)
        {
          string = realloc(string, length + INITIAL_STRING_LENGTH);
          if (string == NULL)
          {
            printf("ERROR. Could not read from file, not enough memory.\n");
            free(string);
            return JSON_ERROR;
          }
          continue;
        }
        continue;
      }
      continue;
    }

    // Close the string and break.
    string[position] = '\0';
    break;
  }

  return string;
}


// Receives a JSON string and tries to parse it to a structure.
// Returns a pointer to the structure if successful.
// Returns NULL if not successful.
static NODE *parse(char *string)
{
  NODE *root = createNode();
  if (root == NULL)
  {
    free(root);
    return NULL;
  }
  jsonModify(root, "key", "root");
  jsonModify(root, "type", JSON_OBJECT);

  // Start reading the string.
  int position = 1;
  char character;
  while (1)
  {
    // Get the next character.
    character =
  }
}


// Receives a pointer to the char where the node begins, and the length of the node.
// Returns a node pointer if successful.
static NODE *parseNode(char *string, int length)
{
  int position;

  // The first thing in the node is the key.
  int length = findString(&string[1], length - 1);
  if (length == -1)
  {
    printf("ERROR: Could not find the key in the node.\n");
    return NULL;
  }

  // Store the key; length is the length of the key.
  char key[STRING_LENGTH + 1];
  memcpy(key, string + 1, length);
  key[length] = '\0';

  // Get the position of the value in the node.
  position = length + 3;

  // Create some space for the node.
  NODE *node = malloc(sizeof(NODE));
  initializeNode(node);
  // TODO: wrap this with abstraction.
  setKey(node, key);
  //jsonModify(node, "key", key);

  // Access the character in position to get the type.
  if (string[position] == 'n')
  {
    // This is a null node. Nothing to do.
  }
  else if (string[position] == '\"')
  {
    // This is a string node. Find the string.
    int valueLength = findString(&string[position + 1], length - position -1);
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
    int numberLength = findEndOfNumber(&string[position], length - position);
    char numberString[STRING_LENGTH + 1];
    memcpy(numberString, &string[position], numberLength);
    numberString[numberLength] = '\0';
    //int number = atoi(numberString);

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
    // This is an object node.
  }
  else
  {
    // Error parsing the node.
    return NULL;
  }
}

// Receives a pointer to a buffer of chars and the length of the buffer.
// Returns the number of characters before the \" character (length of the string).
static int findString(char *string, int length)
{
  int position = 0;
  while (position < length && string[position] != '\"')
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
static inc findEndOfNumber(char *string, int length)
{
  int position = 0;
  while (position < length && isdigit(string[position]))
  {
    position++;
  }

  return position;
}
