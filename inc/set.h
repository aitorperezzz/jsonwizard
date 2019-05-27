#ifndef SET_H
#define SET_H

// Includes.
#include "wizard.h"

// Prototypes of public functions.
int jsonModify(node_t *, char *, char *, char *);
int setType(node_t *, int);
int setKey(node_t *, char *);
int setData(node_t *, char *);
int setChildNumber(node_t *, int);

#endif
