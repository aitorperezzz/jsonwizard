#include "read_lex.h"
#include "read_sm.h"
#include "read_sm_define.h"

enum StateMachineId
{
    STATE_MACHINE_STRING,
    STATE_MACHINE_NUMBER,
    STATE_MACHINE_TRUE,
    STATE_MACHINE_FALSE,
    STATE_MACHINE_NULL,
    STATE_MACHINE_WHITESPACE,
    STATE_MACHINE_TOTAL
};

typedef struct StateMachineInit_st
{
    enum StateMachineId id;
    StateMachine *(*init_callback)(void);
    bool add_token;
    enum TokenId token_id;
} StateMachineInit;

// Callbacks for each of the state machines, as well as token ids on success
StateMachineInit state_machines_init[STATE_MACHINE_TOTAL] = {
    {STATE_MACHINE_STRING, read_sm_define_string, true, TOKEN_ID_STRING},
    {STATE_MACHINE_NUMBER, read_sm_define_number, true, TOKEN_ID_NUMBER},
    {STATE_MACHINE_TRUE, read_sm_define_true, true, TOKEN_ID_TRUE},
    {STATE_MACHINE_FALSE, read_sm_define_false, true, TOKEN_ID_FALSE},
    {STATE_MACHINE_NULL, read_sm_define_null, true, TOKEN_ID_NULL},
    {STATE_MACHINE_WHITESPACE, read_sm_define_whitespace, false},
};

typedef struct ReservedCharInit_st
{
    char character;
    enum TokenId id;
} ReservedCharInit;

#define RESERVED_CHAR_INIT_LENGTH 6
static const ReservedCharInit reserved_chars_init[RESERVED_CHAR_INIT_LENGTH] = {
    {':', TOKEN_ID_COLON},
    {',', TOKEN_ID_COMMA},
    {'{', TOKEN_ID_LEFT_BRACE},
    {'}', TOKEN_ID_RIGHT_BRACE},
    {'[', TOKEN_ID_LEFT_BRACKET},
    {']', TOKEN_ID_RIGHT_BRACKET},
};

// List of state machines. Call initialise function to initialise them
StateMachine *state_machines[STATE_MACHINE_TOTAL];

static enum TokenId state_machine_id_to_token_id(const enum StateMachineId id)
{
    switch (id)
    {
    case STATE_MACHINE_STRING:
        return TOKEN_ID_STRING;
    case STATE_MACHINE_NUMBER:
        return TOKEN_ID_NUMBER;
    case STATE_MACHINE_TRUE:
        return TOKEN_ID_TRUE;
    case STATE_MACHINE_FALSE:
        return TOKEN_ID_FALSE;
    case STATE_MACHINE_NULL:
        return TOKEN_ID_NULL;
    default:
        return TOKEN_ID_TOTAL;
    }
}

ResultCode read_lex_initialise()
{
    // Create all the state machines
    for (size_t i = 0, n = STATE_MACHINE_TOTAL; i < n; i++)
    {
        state_machines[i] = state_machines_init[i].init_callback();
        if (state_machines[i] == NULL)
        {
            return CODE_MEMORY_ERROR;
        }
    }
    return CODE_OK;
}

ResultCode read_lex(const String *string, Vector *tokens)
{
    if (string == NULL || tokens == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Clear the vector provided just in case
    types_vector_clear(tokens);

    String *remaining_string = types_string_copy(string);
    while (types_string_length(remaining_string))
    {
        // Check if this is one of the reserved characters
        bool reserved_character_success = false;
        if (types_string_length(remaining_string) >= 1)
        {
            const char current_char = types_string_at(remaining_string, 0);
            for (size_t i = 0, n = RESERVED_CHAR_INIT_LENGTH; i < n; i++)
            {
                if (current_char == reserved_chars_init[i].character)
                {
                    // This is one of the reserved characters

                    // Push the token
                    Token new_token;
                    new_token.id = reserved_chars_init[i].id;
                    types_vector_push(tokens, &new_token);

                    // Consume the character
                    String *new_string = types_string_create_from_buffer(types_string_c_str(remaining_string) + 1, types_string_length(remaining_string) - 1);
                    types_string_free(remaining_string);
                    free(remaining_string);
                    remaining_string = new_string;

                    // Get out of this loop
                    break;
                }
            }
        }
        if (reserved_character_success)
        {
            continue;
        }

        // Check if this matches any of the state machines we have defined for the lexer
        ResultCode result;
        bool success;
        size_t offset;
        bool state_machine_success = false;
        for (size_t i = 0, n = STATE_MACHINE_TOTAL; i < n; i++)
        {
            result = read_sm_execute(state_machines[i], remaining_string, &success, &offset);
            if (result != CODE_OK)
            {
                return CODE_MEMORY_ERROR;
            }
            else if (success)
            {
                state_machine_success = true;

                // If a token is specified, we need to include it in the token of vectors
                if (state_machines_init[i].add_token)
                {
                    Token new_token;
                    new_token.id = state_machine_id_to_token_id(state_machines_init[i].id);
                    // If a string or a number, I still need to fill in the data.
                    if (new_token.id == TOKEN_ID_STRING || new_token.id == TOKEN_ID_NUMBER)
                    {
                        new_token.data = types_string_create_from_buffer(types_string_c_str(remaining_string) + 1, offset - 1);
                        if (new_token.data == NULL)
                        {
                            return CODE_MEMORY_ERROR;
                        }
                    }

                    // Add the token to the list
                    types_vector_push(tokens, &new_token);
                }

                // The state machine has been successful, so we need to consume the string
                String *new_string = types_string_create_from_buffer(remaining_string->buffer + offset, types_string_length(remaining_string) - offset);
                types_string_free(remaining_string);
                free(remaining_string);
                remaining_string = new_string;

                // Get out of this loop
                break;
            }
        }
        if (state_machine_success)
        {
            continue;
        }

        // If we have reached here, it means we have some string left that does not match anything,
        // so this is a failure
        return CODE_LOGIC_ERROR;
    }

    return CODE_OK;
}

ResultCode read_lex_free_token(void *token_raw)
{
    if (token_raw == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    Token *token = (Token *)token_raw;
    token->id = 0;
    free(token->data);
    token->data = NULL;
    return CODE_OK;
}