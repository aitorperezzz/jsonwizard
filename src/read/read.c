#include "read.h"
#include "read_sm.h"
#include "read_sm_define.h"
#include "read_lex.h"
#include "read_parse.h"

ResultCode read_initialise()
{
    // Initialise the lexer
    if (read_lex_initialise() != CODE_OK)
    {
        return CODE_ERROR;
    }

    // Initialise the parser

    return CODE_OK;
}

Node *read_from_file(const String *filename)
{
    return NULL;
}

Node *read_from_string(const String *string)
{
    if (string == NULL || string->buffer == NULL)
    {
        return NULL;
    }

    if (string->length == 0)
    {
        return NULL;
    }

    // Go through the lexer
    Vector *tokens = types_vector_create(sizeof(Token), read_lex_free_token);
    if (read_lex(string, tokens) != CODE_OK)
    {
        return NULL;
    }

    // Go through the parser
    Node *node = read_parse(tokens);
    return node;
}