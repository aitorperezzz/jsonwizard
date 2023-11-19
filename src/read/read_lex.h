#ifndef READ_LEX_H
#define READ_LEX_H

#include "utils.h"
#include "types/types_string.h"
#include "types/types_vector.h"

enum TokenId
{
    TOKEN_ID_STRING,
    TOKEN_ID_NUMBER,
    TOKEN_ID_TRUE,
    TOKEN_ID_FALSE,
    TOKEN_ID_NULL,
    TOKEN_ID_COLON,
    TOKEN_ID_COMMA,
    TOKEN_ID_LEFT_BRACE,
    TOKEN_ID_RIGHT_BRACE,
    TOKEN_ID_LEFT_BRACKET,
    TOKEN_ID_RIGHT_BRACKET,
    TOKEN_ID_TOTAL
};

typedef struct Token_st
{
    enum TokenId id;
    String *data;
} Token;

ResultCode read_lex_initialise();

ResultCode read_lex(const String *string, Vector *tokens);

ResultCode read_lex_free_token(void *token_raw);

#endif