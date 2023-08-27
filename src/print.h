#ifndef PRINT_H
#define PRINT_H

#include "node.h"
#include "utils.h"

ResultCode print_to_file(const Node *node, const String *filename);
ResultCode print_to_stdout(const Node *node);

#endif
