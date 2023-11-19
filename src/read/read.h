#ifndef READ_H
#define READ_H

#include "utils.h"
#include "node.h"

ResultCode read_initialise();

Node *read_from_file(const String *filename);

Node *read_from_string(const String *string);

#endif