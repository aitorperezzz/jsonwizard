#ifndef READ_SM_DEFINE_H
#define READ_SM_DEFINE_H

#include "read_sm.h"

StateMachine *read_sm_define_string();

StateMachine *read_sm_define_number();

StateMachine *read_sm_define_true();

StateMachine *read_sm_define_false();

StateMachine *read_sm_define_null();

StateMachine *read_sm_define_whitespace();

#endif