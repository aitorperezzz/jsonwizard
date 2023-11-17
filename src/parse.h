#ifndef PARSE_H
#define PARSE_H

#include "utils.h"
#include "types/types_string.h"
#include "types/types_vector.h"
#include "json_path.h"

enum Command
{
    COMMAND_SET_VALUE,
    COMMAND_SET_KEY,
    COMMAND_ERASE
};

struct CommandSetValueData
{
    String value;
};

struct CommandSetKeyData
{
    String key;
};

union CommandData
{
    struct CommandSetValueData set_value_data;
    struct CommandSetKeyData set_key_data;
};

typedef struct ParsedCommand_st
{
    Vector *words;
    String *filename;
    enum Command command;
    JsonPath path;
    union CommandData data;
} ParsedCommand;

ParsedCommand *parse(const String *string);

#endif