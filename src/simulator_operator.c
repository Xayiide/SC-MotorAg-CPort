#include <stdlib.h>    /* abs       */
#include <math.h>      /* fabs      */
#include <stdbool.h>   /* bool      */
#include "sim_types.h" /* BAD_SCORE */
#include "dc_motor_sim.h"
#include "controller.h"
#include "simulator_operator.h"


float simop_simulate(float kp, float ki, float kd)
{
    float score;
    float speed[2000]; /* samples during 0.2s */   
    int   tr = 0;
    //int   ts = 0;
    float mp = 0;
    float aux_mp = 0;

    const float ref = 80.0; /* rad/s */
    const int   expected_tr = 140; /* 14 ms */
    //const int   expected_tp = 140; /* 14 ms */
    const float expected_mp = 0.07 * ref; /* 7 % */
    //const int   expected_ts = 900; /* 10 ms */

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
    for (i = 0; i < 2000; i++)
        speed[i] = 0.0;
        
    ctrl_exec_cycle(&ctrl, &dcmotor);
    dcmsim_exec_sim_cycle(&dcmotor);

    /* Calculate each sample (0.2 s each 0.1 ms means 2000 samples) */
    for (i = 0; i < 2000; i++) {
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

    if (abort_sim) {
        score = BAD_SCORE;
    }
    else {
        for (i = 1; i < 2000; i++) {
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
