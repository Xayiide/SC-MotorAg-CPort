#include "controller.h"
#include "dc_motor_sim.h"

static float kp, ki, kd;
static float angular_speed_reference;
static float e[2];
static float ei, ed;


void init_controller(float new_kp, float new_ki, float new_kd) {
    kp   = new_kp;
    ki   = new_ki;
    kd   = new_kd;

    e[0] = 0.0;
    e[1] = 0.0;
    ei   = 0.0;
    ed   = 0.0;
}

void set_reference(float angular_speed) {
    angular_speed_reference = angular_speed;
}

void exec_controller_cycle() {
    float motor_speed;
    float new_motor_voltage;
    float controller_t = 0.001;

    motor_speed = get_speed();
    e[1] = angular_speed_reference - motor_speed;
    ei = ei + e[1];
    ed = (e[1] - e[0])/controller_t;

    e[0] = e[1];

    new_motor_voltage = (kp * e[1]) + (ki * ei) + (kd * ed);
    set_ea(new_motor_voltage);
}

