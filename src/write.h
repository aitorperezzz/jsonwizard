#ifndef WRITE_H
#define WRITE_H

#include "node.h"
#include "utils.h"

String *write_to_string(const Node *node);
ResultCode write_to_file(const Node *node, const String *filename);

#endif
