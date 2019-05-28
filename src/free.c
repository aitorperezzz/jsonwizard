#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "free.h"
#include "set.h"

// Implementation of public functions.

// Deletes the optionally selected node from the tree.
// If no key is given, the whole tree is deleted.
int jsonDelete(node_t **rootAddress, char *what)
{
  // Check the root node is available.
  if (*rootAddress == NULL)
  {
    printf("ERROR: cannot delete node in a NULL root node.\n");
    return JSON_ERROR;
  }

  // Decide if the user has given the optional parameter.
  char key[STRING_LENGTH + 1];
  if (strcmp(what, "") == 0)
  {
    // The user wants to delete the complete root.
    strcpy(key, "root");
  }
  else
  {
    // A key has been provided.
    strcpy(key, what);
  }

  // Try to find the relevant node to delete.
  node_t *node = searchByKey(*rootAddress, key);
  if (node == NULL)
  {
    printf("ERROR: cannot delete node. Key %s was not found.\n", key);
    return JSON_ERROR;
  }

  // Try to get the parent node.
  node_t *parent = node->parent;

  if (parent == NULL)
  {
    if (strcmp(node->key, "root") == 0)
    {
      // The user wants to delete the root tree.
      freeNode(*rootAddress);
      *rootAddress = createRoot();
      return JSON_OK;
    }
    else
    {
      printf("ERROR: cannot delete node. Parent was not found.\n");
      return JSON_ERROR;
    }
  }

  // Search the position of this node in the parent's childs.
  objData_t *parentData = (objData_t *) parent->data;
  int i;
  for (i = 0; i < parentData->childNumber; i++)
  {
    if (strcmp(parentData->objects[i]->key, node->key) == 0)
    {
      break;
    }
  }
  if (i == parentData->childNumber)
  {
    printf("ERROR: could not delete node. Index of child not found.\n");
    return JSON_ERROR;
  }

  // Move the childs to the left from the i-th.
  for (int j = i; j < parentData->childNumber; j++)
  {
    parentData->objects[j] = parentData->objects[j + 1];
  }
  setChildNumber(parent, parentData->childNumber - 1);
  freeNode(node);

  return JSON_OK;
}


// Quits the program.
int jsonQuit(node_t *root, char **command)
{
  // Free the root structure.
  freeNode(root);

  // Free the command buffer.
  for (int i = 0; i < COMMAND_WORDS; i++)
  {
    free(command[i]);
  }

  return JSON_QUIT;
}


// Deletes only the data of a node if necessary.
int freeData(node_t *node)
{
  // Check the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot free data in NULL node.\n");
    return JSON_ERROR;
  }

  // Check if there is actually some data.
  if (node->data == NULL)
  {
    // Nothing to do.
    return JSON_OK;
  }

  // Now decide according to the type of node.
  switch (node->type)
  {
    case TYPE_NULL:
      return JSON_OK;
    case TYPE_STRING:
    case TYPE_INTEGER:
    case TYPE_BOOLEAN:
      free(node->data);
      node->data = NULL;
      return JSON_OK;
    case TYPE_OBJECT:
      // Free the child objects.
      ;
      objData_t *data = (objData_t *) node->data;
      for (int i = 0; i < data->childNumber; i++)
      {
        freeNode(data->objects[i]);
      }

      // Free the object data structure itself.
      free(data);
      return JSON_OK;
    case TYPE_ARRAY:
      // TODO: free the data of an array node.
      return JSON_ERROR;
    default:
      return JSON_ERROR;
  }
}


// Receives a pointer to a node and frees it recursively.
int freeNode(node_t *node)
{
  // Check that the node is available.
  if (node == NULL)
  {
    printf("ERROR: cannot free NULL node.\n");
    return JSON_ERROR;
  }

  freeData(node);
  free(node);

  return JSON_ERROR;
}
