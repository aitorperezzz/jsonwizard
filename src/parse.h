#ifndef PARSE_H
#define PARSE_H

#include "utils.h"
#include "string_type.h"
#include "vector.h"
#include "json_path.h"

enum Command
{
    COMMAND_SET_VALUE,
    COMMAND_SET_KEY,
    COMMAND_ERASE
};

struct CommandSetValueData
{
    JsonPath path;
    String value;
};

struct CommandSetKeyData
{
    JsonPath path;
    String key;
};

struct CommandEraseData
{
    JsonPath path;
};

union CommandData
{
    struct CommandSetValueData setValueData;
    struct CommandSetKeyData setKeyData;
    struct CommandEraseData eraseData;
};

typedef struct ParsedCommand_st
{
    Vector *words;
    String *filename;
    enum Command command;
    union CommandData data;
} ParsedCommand;

ParsedCommand *parse(const String *string);

#endif