#ifndef JSONWIZARD_H
#define JSONWIZARD_H

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
  JSON_NULL, /* Node does not yet have a valid type. */
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


// Normal node structure.
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
struct arrayNode_st
{
  char key[STRING_LENGTH + 1];
  struct arrayNode_st *next;
  void *data;
};
typedef struct arrayNode_st ARRAYNODE;


// TODO: organise these functions into modules.
// Prototypes.
int booleanTypeToString(char *, BOOLEAN);
NODE *searchByKey(NODE *, char *);

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

int jsonAppend(NODE **, char *, char *);

int jsonModify(NODE *, char *, char *, char *);
int typeStringToCode(char *);
int setData(NODE *, char *);
int getBooleanFromString(char *data);
int initializeObjectData(OBJDATA *objData);

int jsonDelete(NODE **, char *);
int jsonFreeNode(NODE *);

#endif
