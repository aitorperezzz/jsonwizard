#include "read_sm.h"

static ResultCode read_sm_free(void *data)
{
    // The data pointed at is itself a StateMachine
    if (data == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    StateMachine *sm = (StateMachine *)data;
    if (sm->states != NULL)
    {
        types_vector_free(sm->states);
    }
    sm->acceptance_state = 0;
    return CODE_OK;
}

StateMachine *read_sm_create()
{
    StateMachine *sm = malloc(sizeof(StateMachine));
    if (sm == NULL)
    {
        return NULL;
    }
    sm->states = types_vector_create(sizeof(StateMachine), read_sm_free);
    if (sm->states == NULL)
    {
        free(sm);
        return NULL;
    }
    sm->acceptance_state = 0;
    return sm;
}

ResultCode read_sm_add_state(StateMachine *sm, int id)
{
    if (sm == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    if (sm->states == NULL)
    {
        return CODE_MEMORY_ERROR;
    }
    types_vector_push(sm->states, &id);
    return CODE_OK;
}

ResultCode read_sm_add_tranistions(StateMachine *sm, TransitionDef *transition_defs, const size_t number)
{
    if (sm == NULL || transition_defs == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    for (size_t i = 0; i < number; i++)
    {
        TransitionDef transition_def = transition_defs[i];

        // Check the origin and destination states have been defined
        if (types_vector_size(sm->states) < transition_def.origin || types_vector_size(sm->states) < transition_def.destination)
        {
            return CODE_LOGIC_ERROR;
        }

        // Check the callback function is defined
        if (transition_def.callback == NULL)
        {
            return CODE_MEMORY_ERROR;
        }

        // Access the origin state (the index in the vector is the same as the id)
        State *origin_state = types_vector_at(sm->states, transition_def.origin);

        // Create the transition to add to the vector
        Transition transition;
        transition.callback = transition_def.callback;
        transition.destination = transition_def.destination;

        // Push to the other transitions
        types_vector_push(origin_state->transitions, &transition);
    }

    return CODE_OK;
}

ResultCode read_sm_define_acceptance_state(StateMachine *sm, int state)
{
    if (sm == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Check the acceptance state has been defined
    if (types_vector_size(sm->states) <= state)
    {
        return CODE_LOGIC_ERROR;
    }

    // Set the acceptance state
    sm->acceptance_state = state;
    return CODE_OK;
}

ResultCode read_sm_execute(const StateMachine *sm, const String *string, bool *success, size_t *offset)
{
    if (sm == NULL || string == NULL || success == NULL || offset == NULL)
    {
        return CODE_MEMORY_ERROR;
    }

    // Iterate over the complete string
    State *state = types_vector_at(sm->states, 0);
    for (size_t i = 0, n = types_string_length(string); i < n; i++)
    {
        // For each of the transitions of the current state,
        // call the callback to see it matches the condition
        Transition *transition_found = NULL;
        for (size_t j = 0, m = types_vector_size(state->transitions); j < m; j++)
        {
            Transition *transition = types_vector_at(state->transitions, j);
            if (transition->callback(types_string_at(string, i)))
            {
                // The condition matches, so break
                transition_found = transition;
                break;
            }
        }

        // If no transition has been found (and this is not the acceptance state)
        // then return with an error
        if (!transition_found)
        {
            *success = false;
            *offset = i;
            return CODE_ERROR;
        }

        // If we have found the acceptance state, return with success
        if (transition_found->destination == sm->acceptance_state)
        {
            *success = true;
            *offset = i;
            return CODE_OK;
        }

        // The transition was found, so move the state machine
        state = types_vector_at(state->transitions, transition_found->destination);
    }

    // The string provided has been completed without finding the acceptance
    // state, so return with an error
    *success = false;
    *offset = types_string_length(string);
    return CODE_ERROR;
}