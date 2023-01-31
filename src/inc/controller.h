#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "sim_types.h"

void init_controller(float, float, float);

void set_reference(float);

void exec_controller_cycle();

#endif
