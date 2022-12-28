#ifndef FREE_H
#define FREE_H

#include "wizard.h"

// Prototypes for the public functions of the module.
int jsonDelete(Node **, const String *key);
int jsonQuit(Node *, Vector *words);
int freeData(Node *);
int freeNode(Node *node);

#endif
