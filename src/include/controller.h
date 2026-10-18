#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "dc_motor_sim.h"

struct controller {
    float kp, ki, kd;
    float angular_speed_ref;
    float e[2];
    float ei, ed;
};

void ctrl_init(struct controller *c, float new_kp, float new_ki, float new_kd);
void ctrl_set_reference(struct controller *c, float angular_speed);
void ctrl_exec_cycle(struct controller *c, struct dcmsim_info *i);

#endif
