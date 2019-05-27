// This module modifies fields inside a node.

// Includes.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "free.h"

// Prototypes of static functions.
static int validateInteger(char *);
static int initializeData(node_t *);
static int initializeObjectData(objData_t *);


// Implementation of public functions.

// This function modifies the node indicated by its key.
int jsonModify(node_t *root, char *key, char *field, char *value)
{
  // Check root is available.
  if (root == NULL)
  {
    printf("ERROR: cannot modify node if root is NULL.\n");
    return JSON_ERROR;
  }

  // Try to find the relevant node.
  node_t *node = searchByKey(root, key);
  if (node == NULL)
  {
    printf("ERROR: cannot modify node. Key %s was not found.\n", key);
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
    printf("ERROR: cannot modify node. '%s' is not a valid field.\n", field);
    return JSON_ERROR;
  }
}


// Tries to change the type of a node.
int setType(node_t *node, int type)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot set type in NULL node.\n");
    return JSON_ERROR;
  }

  // Validate the type provided.
  if (validateType(type) == JSON_ERROR)
  {
    printf("ERROR: cannot change type of node. Type is not valid.\n");
    return JSON_ERROR;
  }

  // First free the data of the node.
  freeData(node);

  // Set the new type and initialize.
  node->type = type;
  return initializeData(node);
}


// Receives a key and sets it for the node.
int setKey(node_t *node, char *key)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot set key in NULL node.\n");
    return JSON_ERROR;
  }

  // Check a key was provided.
  if (key == NULL)
  {
    printf("ERROR: cannot set key in node. No key was provided.\n");
    return JSON_ERROR;
  }

  // TODO: check the key is not already in use.

  strcpy(node->key, key);
  return JSON_OK;
}


// Receives data for a node as a string.
// Only available for certain types of nodes.
int setData(node_t *node, char *data)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot set data in NULL node.\n");
    return JSON_ERROR;
  }

  // Check that data was provided.
  if (data == NULL)
  {
    printf("ERROR: cannot set data in node. No data was provided.\n");
    return JSON_ERROR;
  }

  // Check the node is of the correct type.
  switch (node->type)
  {
    case TYPE_NULL:
      printf("ERROR: cannot set data in a node of type NULL.\n");
      printf("Change its type first.\n");
      return JSON_ERROR;
    case TYPE_OBJECT:
    case TYPE_ARRAY:
      printf("ERROR: cannot modify data inside ARRAY and OBJECT nodes with setData.\n");
      return JSON_ERROR;
    case TYPE_STRING:
      strcpy((char *) node->data, data);
      return JSON_OK;
    case TYPE_INTEGER:
      if (validateInteger(data) == JSON_ERROR)
      {
        printf("ERROR: cannot set data. Not a valid integer value.\n");
        return JSON_ERROR;
      }
      *((int *) node->data) = atoi(data);
      return JSON_OK;
    case TYPE_BOOLEAN:
      if (strcmp(data, "true") == 0 || strcmp(data, "false") == 0)
      {
        int boolean = booleanStringToCode(data);
        *((boolean_t *) node->data) = boolean;
        return JSON_OK;
      }
      else
      {
        printf("ERROR: cannot set data. '%s' is not a valid boolean value.\n", data);
        return JSON_ERROR;
      }
    default:
      printf("ERROR: cannot change data in node of unknown type.\n");
      return JSON_ERROR;
  }
}


// Sets the child number of a node of type object.
int setChildNumber(node_t *node, int number)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot change child number in NULL node.\n");
    return JSON_ERROR;
  }

  // Check the node is of type object.
  if (node->type != TYPE_OBJECT)
  {
    printf("ERROR. cannot change child number. Node is not of type OBJECT.\n");
    return JSON_ERROR;
  }

  ((objData_t *) node->data)->childNumber = number;
  return JSON_OK;
}


// Receives a number as a string and checks if it is valid.
static int validateInteger(char *string)
{
  for (int i = 0, n = strlen(string); i < n; i++)
  {
    if (!isdigit(string[i]))
    {
      return JSON_ERROR;
    }
  }

  return JSON_OK;
}


// Presuposes the data of the node is NULL and reserves memory
// according to the type of the node.
static int initializeData(node_t *node)
{
  // Check if the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot reset data of NULL node.\n");
    return JSON_ERROR;
  }

  // Check that the data is NULL, as expected.
  if (node->data != NULL)
  {
    printf("ERROR: cannot initialize data of node. Data is not NULL.\n");
    return JSON_ERROR;
  }

  // Decide depending on the type of node.
  switch (node->type)
  {
    case TYPE_NULL:
      return JSON_OK;
    case TYPE_STRING:
      // Create space for a string and initialize it to empty.
      node->data = malloc(STRING_LENGTH + 1);
      strcpy((char *) node->data, "");
      return JSON_OK;
    case TYPE_INTEGER:
      // Create space for an integer and store a zero.
      node->data = malloc(sizeof(int));
      *((int *) node->data) = 0;
      return JSON_OK;
    case TYPE_BOOLEAN:
      // Create space for a boolean (integer) and store unkown.
      node->data = malloc(sizeof(boolean_t));
      *((boolean_t *) node->data) = BOOL_UNKNOWN;
      return JSON_OK;
    case TYPE_OBJECT:
      // Initialize an object data structure.
      node->data = malloc(sizeof(objData_t));
      initializeObjectData((objData_t *) node->data);
      return JSON_OK;
    case TYPE_ARRAY:
      // TODO: reset data in an array node.
      return JSON_ERROR;
    default:
      printf("ERROR: could not reset data in node. Type unknown.\n");
      return JSON_ERROR;
  }
}


// Initializes the data structure when the node is of type OBJECT.
static int initializeObjectData(objData_t *objData)
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
    objData->objects[i] = NULL;
  }

  return JSON_OK;
}
