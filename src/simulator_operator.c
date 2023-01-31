#include <stdlib.h>    /* abs       */
#include <math.h>      /* fabs      */
#include <stdbool.h>   /* bool      */
#include "sim_types.h" /* BAD_SCORE */
#include "dc_motor_sim.h"
#include "controller.h"
#include "simulator_operator.h"


float carry_out_a_simulation(float kp, float ki, float kd) {
    const float ref = 80.0; /* rad/s */
    const int   expected_tr = 140; /* 14 ms */
    const float expected_mp = ref * 1.07; /* 7% (85.8 for 80 rad/s) */
    float score;
    float speed[2000]; /* samples during 0.2s */   
    bool  ref_reached = false;
    bool  tp_checked  = false;
    bool  abort_sim   = false;
    int   tr;
    float mp;
    float aux_mp;
    int   i;

    init_dc_motor_sim();
    init_controller(kp, ki, kd);

    set_reference(ref);

    for (i = 0; i < 2000; i++)
        speed[i] = 0.0;
        
    exec_controller_cycle();
    exec_sim_cycle();

    for (i = 0; i < 2000; i++) {
        speed[i] = get_speed();

        /* extreme values */
        if ((speed[i] > (ref * 5.0)) || (speed[i] < 0.0)) {
            abort_sim = true;
            break;
        }

        if (i % 10 == 0) /* controller runs every milisecond */
            exec_controller_cycle();

        exec_sim_cycle();
    }

    if (abort_sim) {
        score = BAD_SCORE;
    }
    else {
        tr = 0;
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

        score = (float)(abs(tr - expected_tr) + fabs(mp - expected_mp));
    }
    return score;
}
