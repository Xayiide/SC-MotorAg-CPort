#include "dc_motor_sim.h"

static float motor_t;
static float w_ec_w_factor, w_ec_i_factor;
static float i_ec_v_factor, i_ec_i_factor;

static float R, L, J, fv, K, ea;
static float i[2];
static float w[2];


void init_dc_motor_sim() {
    motor_t = 0.0001;
    R  = 1.11;    /* Ohm               */
    L  = 0.0002;  /* H                 */
    J  = 6.77E-6; /* Kgm^2             */
    fv = 1.66E-5; /* Nm/(Rad/s)        */
    K  = 0.0364;  /* Nm/A or V/(Rad/s) */

    ea   = 0.0;
    w[0] = 0.0;
    w[1] = 0.0;
    i[0] = 0.0;
    i[1] = 0.0;

    
    w_ec_w_factor = (float)1.0 - (motor_t * fv)/(J);
    w_ec_i_factor = (motor_t * K)/(J);

    i_ec_v_factor = (motor_t)/(L);
    i_ec_i_factor = (float)1.0 - (motor_t * R)/(L);
}

void set_ea(float new_ea) {
    ea = new_ea;
    if (ea > 24.0)
        ea = 24.0;
    else if (ea < -24.0)
        ea = -24.0;
}

float get_speed() {
    return w[0];
}

void exec_sim_cycle() {
    w[0] = w[1];
    i[0] = i[1];
    w[1] = w_ec_w_factor * w[0] + w_ec_i_factor * i[0];
    i[1] = i_ec_i_factor * i[0] + i_ec_v_factor * (ea - K * w[0]);
}
