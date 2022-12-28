#ifndef SET_H
#define SET_H

#include "wizard.h"
#include "utils.h"
#include "node.h"

ResultCode jsonModify(Node *root, const String *key, const String *field, const String *value);
ResultCode setType(Node *node, NodeType type);
ResultCode setKey(Node *node, const String *key);
ResultCode setData(Node *node, const String *value);

// Validation and manipulation of basic types.
int validateType(NodeType type);
int validateField(int);
Boolean booleanStringToCode(const String *string);
String *booleanCodeToString(Boolean code);
NodeType typeStringToCode(const String *string);

#endif
