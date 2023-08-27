#ifndef WIZARD_H
#define WIZARD_H

#include "string_type.h"
#include "node.h"
#include "utils.h"
#include "vector.h"

// Parsing and execution
ResultCode printHelp();
ResultCode parseCommand(const String *input, Vector *words);
ResultCode executeCommand(const Vector *command, Node **rootAddress);

// Creation of nodes.
Node *createNode(void);
Node *createRoot(void);
ResultCode initializeNode(Node *);

// Append.
ResultCode jsonAppend(Node **rootAddress, const String *parentKey, const String *childKey);

#endif
