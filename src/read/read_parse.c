#include "read_parse.h"
#include "read_lex.h"
#include "types/types_map.h"

static NodeType token_id_to_node_type(const enum TokenId id);
static Node *read_parse_value(const Iterator first, const Iterator last);
static Node *read_parse_object(const Iterator first, const Iterator last);
static Node *read_parse_array(const Iterator first, const Iterator last);
static ResultCode read_parse_find_token(const Iterator first, const Iterator last,
                                        const enum TokenId token_id, Iterator *result);
static ResultCode read_parse_find_closing_token(const Iterator first, const Iterator last,
                                                const enum TokenId opening_token_id, const enum TokenId closing_token_id, Iterator *result);

Node *read_parse(const Vector *tokens)
{
    if (tokens == NULL)
    {
        return NULL;
    }

    // A JSON string is a value
    return read_parse_value(types_vector_begin(tokens), types_vector_end(tokens));
}

static NodeType token_id_to_node_type(const enum TokenId id)
{
    switch (id)
    {
    case TOKEN_ID_STRING:
        return NODE_TYPE_STRING;
    case TOKEN_ID_NUMBER:
        return NODE_TYPE_NUMBER;
    case TOKEN_ID_TRUE:
        return NODE_TYPE_TRUE;
    case TOKEN_ID_FALSE:
        return NODE_TYPE_FALSE;
    case TOKEN_ID_NULL:
        return NODE_TYPE_NULL;
    default:
        return NODE_TYPE_NULL;
    }
}

static Node *read_parse_value(const Iterator first, const Iterator last)
{
    size_t number;
    if (types_iterator_distance(first, last, &number) != CODE_OK)
    {
        return NULL;
    }
    if (number == 0)
    {
        return NULL;
    }

    // Decide according to the token we have
    Token *token = (Token *)types_iterator_get(first);
    switch (token->id)
    {
    case TOKEN_ID_STRING:
    case TOKEN_ID_NUMBER:
    {
        // Check this node has some data
        if (token->data == NULL)
        {
            return NULL;
        }
        // Check this is the only value provided
        if (number != 1)
        {
            return NULL;
        }

        // Create the node to return and copy the data
        Node *node = malloc(sizeof(node));
        if (node == NULL)
        {
            return NULL;
        }
        node->type = token_id_to_node_type(token->id);
        node->data = types_string_copy(token->data);
        return node;
    }
    case TOKEN_ID_TRUE:
    case TOKEN_ID_FALSE:
    case TOKEN_ID_NULL:
    {
        // Create the node to return and copy the data
        Node *node = malloc(sizeof(node));
        if (node == NULL)
        {
            return NULL;
        }
        node->type = token_id_to_node_type(token->id);
        node->data = NULL;
        return node;
    }
    case TOKEN_ID_LEFT_BRACE:
    {
        // Find the matching right brace
        Iterator result;
        if (read_parse_find_closing_token(types_iterator_increase(first, 1), last, TOKEN_ID_LEFT_BRACE, TOKEN_ID_RIGHT_BRACE, &result) != CODE_OK)
        {
            return NULL;
        }

        // Check that the matching right brace is exactly at the end of the provided range
        if (types_iterator_get(result) != types_iterator_get(last))
        {
            return NULL;
        }

        // Ignore the opening and closing tokens and parse the object
        Iterator inner_first = types_iterator_increase(first, 1);
        Iterator inner_last = types_iterator_decrease(last, 1);
        return read_parse_object(inner_first, inner_last);
    }
    case TOKEN_ID_LEFT_BRACKET:
    {
        // Find the matching right bracket
        Iterator result;
        if (read_parse_find_closing_token(types_iterator_increase(first, 1), last, TOKEN_ID_LEFT_BRACKET, TOKEN_ID_RIGHT_BRACKET, &result) != CODE_OK)
        {
            return NULL;
        }

        // Check that the matching right bracket is exactly at the end of the provided range
        if (types_iterator_get(result) != types_iterator_get(last))
        {
            return NULL;
        }

        // Ignore the opening and closing tokens and parse the object
        Iterator inner_first = types_iterator_increase(first, 1);
        Iterator inner_last = types_iterator_decrease(last, 1);
        return read_parse_array(inner_first, inner_last);
    }
    default:
        return NULL;
    }
}

static Node *read_parse_object(const Iterator first, const Iterator last)
{
    // Create the node
    // TODO
    Node *node = node_create();
    node->type = NODE_TYPE_OBJECT;

    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return NULL;
    }

    // An object node could be empty
    if (distance == 1)
    {
        return node;
    }

    Iterator current = first;
    Token *token;
    while (distance > 1)
    {
        // This token has to be a string
        token = (Token *)types_iterator_get(current);
        if (token->id != TOKEN_ID_STRING)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Create the key, which is a string
        String *key = types_string_copy((String *)token->data);

        // Next token has to be a colon
        current = types_iterator_increase(current, 1);
        token = (Token *)types_iterator_get(current);
        if (token->id != TOKEN_ID_COLON)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Find a comma if there is any
        Iterator comma_iterator;
        if (read_parse_find_token(first, last, TOKEN_ID_COMMA, &comma_iterator) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // The end of the current value is the comma or the complete end, whichever comes first
        Iterator value_last;
        if (types_iterator_get(comma_iterator) < types_iterator_get(last))
        {
            value_last = comma_iterator;
        }
        else
        {
            value_last = last;
        }

        // Advance the start of the value as well
        current = types_iterator_increase(current, 1);

        // Check there is at least something to work with inside the value
        size_t value_distance;
        if (types_iterator_distance(current, value_last, &value_distance) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }
        // A value cannot be empty
        if (value_distance == 0)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Create the value
        Node *value = read_parse_value(current, value_last);
        if (value == NULL)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Add element to the node
        if (node_append(node, key, value) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Advance the iterator
        current = value_last;
        if (types_iterator_distance(current, last, &distance) != CODE_OK)
        {
            return NULL;
        }
    }

    return node;
}

static Node *read_parse_array(const Iterator first, const Iterator last)
{
    // Create the node
    // TODO
    Node *node = node_create();
    if (node == NULL)
    {
        return NULL;
    }
    node->type = NODE_TYPE_ARRAY;
    node->data = types_vector_create(sizeof(Node *), node_free);

    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return NULL;
    }

    // An array node could be empty
    if (distance == 1)
    {
        return node;
    }

    Iterator current = first;
    while (distance > 1)
    {
        // Find a comma if there is any
        Iterator result;
        if (read_parse_find_token(current, last, TOKEN_ID_COMMA, &result) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // The end of the value is either the comma, or the end, whichever comes first
        Iterator value_last;
        if (types_iterator_get(result) < types_iterator_get(last))
        {
            value_last = result;
        }
        else
        {
            value_last = last;
        }

        // Advance the start of the value as well
        current = types_iterator_increase(current, 1);

        // Check there is at least something to work with inside the value
        size_t value_distance;
        if (types_iterator_distance(current, value_last, &value_distance) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }
        if (distance == 0)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Create the value
        Node *value = read_parse_value(current, value_last);
        if (value == NULL)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Push back to the array
        if (node_array_push(node, value) != CODE_OK)
        {
            node_free(node);
            free(node);
            return NULL;
        }

        // Advance the iterator
        current = value_last;
        if (types_iterator_distance(current, last, &distance) != CODE_OK)
        {
            return NULL;
        }
    }

    return node;
}

static ResultCode read_parse_find_token(const Iterator first, const Iterator last,
                                        const enum TokenId token_id, Iterator *result)
{
    // Check there are at least some elements to work with
    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    // Iterate to find
    for (size_t i = 0; i < distance; i++)
    {
        Iterator current = types_iterator_increase(first, i);
        Token *token = (Token *)types_iterator_get(current);
        if (token->id == token_id)
        {
            *result = current;
            return CODE_OK;
        }
    }

    // Closing token has not been found, this cannot be
    return CODE_LOGIC_ERROR;
}

static ResultCode read_parse_find_closing_token(const Iterator first, const Iterator last,
                                                const enum TokenId opening_token_id, const enum TokenId closing_token_id, Iterator *result)
{
    // Check there are at least some elements to work with
    size_t distance;
    if (types_iterator_distance(first, last, &distance) != CODE_OK)
    {
        return CODE_LOGIC_ERROR;
    }

    // The number of closing tokens to find is exactly one
    int num_closing_tokens_left = 1;

    // Iterate to find
    for (size_t i = 0; i < distance; i++)
    {
        Iterator current = types_iterator_increase(first, i);
        Token *token = (Token *)types_iterator_get(current);
        if (token->id == closing_token_id)
        {
            if (num_closing_tokens_left == 1)
            {
                *result = current;
                return CODE_OK;
            }
            else
            {
                num_closing_tokens_left -= 1;
            }
        }
        else if (token->id == opening_token_id)
        {
            num_closing_tokens_left += 1;
        }
    }

    // Closing token has not been found, this cannot be
    return CODE_LOGIC_ERROR;
}