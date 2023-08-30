#ifndef READ_H
#define READ_H

#include "utils.h"
#include "node.h"

Node *read_from_file(const String *filename);

Node *read_from_string(const String *string);

#endif
