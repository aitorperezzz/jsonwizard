#ifndef WIZARD_H
#define WIZARD_H

// Define the length of some variables.
#define STRING_LENGTH 100
#define COMMAND_WORDS 20
#define NUMBER_OBJECTS 20

// Result codes.
typedef enum
{
  JSON_OK,
  JSON_ERROR,
  JSON_QUIT,
} resultCode_t;


// Structures.
// Enum for the type of node.
typedef enum
{
  TYPE_NULL, /* node does not yet have a valid type. */
  TYPE_STRING,
  TYPE_INTEGER,
  TYPE_BOOLEAN,
  TYPE_ARRAY,
  TYPE_OBJECT
} nodeType_t;

// Enum for the fields in a node.
typedef enum
{
  FIELD_TYPE,
  FIELD_KEY,
  FIELD_PARENT,
  FIELD_DATA
} nodeField_t;

// Enum for the possible boolean values.
typedef enum
{
  BOOL_TRUE,
  BOOL_FALSE,
  BOOL_UNKNOWN
} boolean_t;


// Structure of a node.
struct node_st
{
  nodeType_t type;
  char key[STRING_LENGTH + 1];
  struct node_st *parent;
  void *data;
};
typedef struct node_st node_t;

// Data pointed to when the node is of type object.
struct objData_st
{
  int childNumber;
  node_t *objects[NUMBER_OBJECTS];
};
typedef struct objData_st objData_t;

// TODO: data pointed to when the node is of type array.

// Prototypes of public functions.
// Parsing and execution.
int printHelp(void);
int resetCommand(char **, int *);
int parseCommand(char *, char **, int *);
int executeCommand(char **, int, node_t **);

// Creation of nodes.
node_t *createNode(void);
node_t *createRoot(void);
int initializeNode(node_t *);

// Validation and manipulation of basic types.
int validateType(int);
int validateField(int);
int booleanStringToCode(char *);
int booleanCodeToString(int, char *);
int typeStringToCode(char *);

// Append.
int jsonAppend(node_t **, char *, char *);

// Search the tree given the key.
node_t *searchByKey(node_t *, char *);





/*
node_t *searchByKey(node_t *, char *);

// PROTOTYPES.
// TODO: clean this part.
int resetCommand(char **, int *);
int parseCommand(char *, char **, int *);
int printCommand(char **, int);
int executeCommand(char **, int, node_t **);
int printHelp(void);

node_t *createnode_t(void);
//int setType(node_t *, int);
int validateType(int);
int validateField(int);
//int setKey(node_t *, char *);
int setChildNumber(node_t *, int);
//int initializeData(node_t *);
int initializenode_t(node_t *);

int validateType(int);
int validateField(int);

int jsonAppend(node_t **, char *, char *);

int jsonModify(node_t *, char *, char *, char *);
int typeStringToCode(char *);
//int setData(node_t *, char *);
int booleanStringToCode(char *);
int booleanCodeToString(int, char *);
//int initializeObjectData(OBJDATA *objData);

int jsonDelete(node_t **, char *);
int jsonFreenode_t(node_t *);

int jsonQuit(node_t *, char **);*/

#endif
