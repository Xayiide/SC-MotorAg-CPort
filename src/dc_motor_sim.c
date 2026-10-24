#include "dc_motor_sim.h"

#define TENSION_MAX_V   24.0f
#define TENSION_MIN_V (-24.0f)

#define US_IN_SECOND (0.000001)

void dcmsim_init(struct dcmsim_info *i)
{
    i->motor_t_s = US_IN_SECOND * DCM_SIM_PERIOD_US; /* 0.0001 seconds */
    i->R  = 1.11;    /* Ohm               */
    i->L  = 0.0002;  /* H                 */
    i->J  = 6.77E-6; /* Kgm^2             */
    i->fv = 1.66E-5; /* Nm/(Rad/s)        */
    i->K  = 0.0364;  /* Nm/A or V/(Rad/s) */

    i->ea   = 0.0;
    i->w[0] = 0.0;   /* tk angular speed */
    i->w[1] = 0.0;   /* t(k+1) angular speed */
    i->i[0] = 0.0;   /* tk current */
    i->i[1] = 0.0;   /* t(k+1) current */

    i->w_ec_w_factor = (float) (1.0 - (i->motor_t_s * i->fv)/(i->J));
    i->w_ec_i_factor = (i->motor_t_s * i->K)/(i->J);

    i->i_ec_v_factor = (i->motor_t_s)/(i->L);
    i->i_ec_i_factor = (float) (1.0 - (i->motor_t_s * i->R)/(i->L));
}

void dcmsim_set_ea(struct dcmsim_info *i, float new_ea)
{
    i->ea = new_ea;
    /* Saturation effect due to battery limits */
    if (i->ea > TENSION_MAX_V)
        i->ea = TENSION_MAX_V;
    else if (i->ea < TENSION_MIN_V)
        i->ea = TENSION_MIN_V;
}

float dcmsim_get_speed(struct dcmsim_info *i)
{
    return i->w[0];
}

void dcmsim_exec_sim_cycle(struct dcmsim_info *i)
{
    i->w[0] = i->w[1];
    i->i[0] = i->i[1];
    i->w[1] = i->w_ec_w_factor * i->w[0] + i->w_ec_i_factor * i->i[0];
    i->i[1] = i->i_ec_i_factor * i->i[0] +
              i->i_ec_v_factor * (i->ea - i->K * i->w[0]);
}
