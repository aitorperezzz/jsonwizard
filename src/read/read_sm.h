#ifndef READ_SM_H
#define READ_SM_H

#include "types/types_vector.h"
#include "types/types_string.h"

typedef struct State_st
{
    int id;              // identifier of the transition, should come from an enum
    Vector *transitions; // Vector of all the possible transitions from this state
} State;

typedef struct Transition_st
{
    int destination;
    bool (*callback)(const char c);
} Transition;

typedef struct StateMachine_st
{
    Vector *states;
    int acceptance_state;
} StateMachine;

typedef struct TransitionDefSt
{
    int origin;
    bool (*callback)(const char c);
    int destination;
} TransitionDef;

StateMachine *read_sm_create();

ResultCode read_sm_add_state(StateMachine *sm, int id);

ResultCode read_sm_add_tranistions(StateMachine *sm, TransitionDef *transition_defs, const size_t number);

ResultCode read_sm_define_acceptance_state(StateMachine *sm, int state);

ResultCode read_sm_execute(const StateMachine *sm, const String *string, bool *success, size_t *offset);

#endif