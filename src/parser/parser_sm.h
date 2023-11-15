
#include "vector.h"

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
    int acceptanceState;
} StateMachine;

typedef struct TransitionDefSt
{
    int origin;
    bool (*callback)(const char c);
    int destination;
} TransitionDef;

StateMachine *
parser_sm_create();

State *parser_sm_add_state(StateMachine *sm, int id);

ResultCode parser_sm_add_tranistion(State *state, Transition *transition);