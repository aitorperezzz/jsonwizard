#ifndef PRINT_H
#define PRINT_H

#include "node.h"
#include "utils.h"

ResultCode printToFile(const Node *root, const String *filename);
ResultCode printToStdin(Node *node, const String *key);

#endif
