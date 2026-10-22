#include "controller.h"
#include "dc_motor_sim.h"

void ctrl_init(struct controller *c, float new_kp, float new_ki, float new_kd)
{
    c->kp   = new_kp;
    c->ki   = new_ki;
    c->kd   = new_kd;

    c->e[0] = 0.0;
    c->e[1] = 0.0;
    c->ei   = 0.0;
    c->ed   = 0.0;
}

void ctrl_set_reference(struct controller *c, float angular_speed)
{
    c->angular_speed_ref = angular_speed;
}

void ctrl_exec_cycle(struct controller *c, struct dcmsim_info *i)
{
    float motor_speed;
    float new_motor_voltage;
    const float controller_t = 0.001;

    motor_speed = dcmsim_get_speed(i);
    c->e[1] = c->angular_speed_ref - motor_speed;
    c->ei = c->ei + c->e[1];
    c->ed = (c->e[1] - c->e[0])/controller_t;

    /* Prepare last error for next cycle */
    c->e[0] = c->e[1];

    new_motor_voltage = (c->kp * c->e[1]) + (c->ki * c->ei) + (c->kd * c->ed);
    dcmsim_set_ea(i, new_motor_voltage);
}

