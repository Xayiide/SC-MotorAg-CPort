#include <stdlib.h>    /* abs       */
#include <math.h>      /* fabs      */
#include <stdbool.h>   /* bool      */

#include "sim_types.h" /* BAD_SCORE */
#include "dc_motor_sim.h"
#include "controller.h"
#include "simulator_operator.h"

#define SIM_INTERVAL_US 200000 /* Simulamos 200 ms */
#define TOTAL_SAMPLES (SIM_INTERVAL_US / DCM_SIM_PERIOD_US)

float simop_simulate(float kp, float ki, float kd)
{
    float score;
    float speed[TOTAL_SAMPLES];
    int   tr = 0;
    float mp = 0;
    float aux_mp = 0;

    const float ref = 80.0; /* rad/s */
    const int   expected_tr = 140; /* 14 ms */
    const float expected_mp = 0.07 * ref; /* 7 % */
    const float max_speed_allowed = ref + expected_mp;

    bool ref_reached = false;
    bool tp_checked  = false;
    bool abort_sim   = false;

    int   i;

    struct dcmsim_info dcmotor;
    struct controller ctrl;

    dcmsim_init(&dcmotor);
    ctrl_init(&ctrl, kp, ki, kd);
    ctrl_set_reference(&ctrl, ref);

    /* Initialize speed */
    for (i = 0; i < TOTAL_SAMPLES; i++)
        speed[i] = 0.0;
        
    ctrl_exec_cycle(&ctrl, &dcmotor);
    dcmsim_exec_sim_cycle(&dcmotor);

    /* Calculate each sample */
    for (i = 0; i < TOTAL_SAMPLES; i++) {
        speed[i] = dcmsim_get_speed(&dcmotor);

        /* Verify extreme values */
        if ((speed[i] > (ref * 5.0)) || (speed[i] < 0.0)) {
            abort_sim = true;
            break;
        }

        if (i % 10 == 0) /* controller runs every milisecond */
            ctrl_exec_cycle(&ctrl, &dcmotor);

        dcmsim_exec_sim_cycle(&dcmotor);
    }

    /* Process all samples to calculate the score of the simulation */
    if (abort_sim) {
        score = BAD_SCORE;
    }
    else {
        for (i = 1; i < TOTAL_SAMPLES; i++) {
            if (ref_reached == false) { /* values in 2..tr */
                if (speed[i] > ref) {
                    ref_reached = true;
                    if (i > 145 || i < 135) /* not near 14 ms */
                        tr = BAD_SCORE_INT;
                    else
                        tr = i;
                }

                if (speed[i] < speed[i-1]) { /* monotonically non-decreasing */
                    tr = BAD_SCORE_INT;
                    break;
                }
            }

            if (ref_reached && tp_checked == false) { /* tp */
                if (speed[i] <= speed[i-1]) {
                    tp_checked = true;
                    aux_mp = speed[i] - ref;
                    if (speed[i] <= expected_mp) /* max mp not passed */
                        mp = (expected_mp - aux_mp); /* mp in (80..85.6) */
                    else /* aux_mp > expected_mp */
                        mp = (aux_mp - expected_mp) * 10.0; /* bad score */
                }
            }
        }
        score = (float) (abs(tr - expected_tr) + fabs(mp - expected_mp));
    }
    return score;
}
