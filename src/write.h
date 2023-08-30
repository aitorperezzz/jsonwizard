#ifndef WRITE_H
#define WRITE_H

#include "node.h"
#include "utils.h"

ResultCode write_to_file(const Node *node, const String *filename);
ResultCode write_to_stdout(const Node *node);

#endif
