#include "ga_operator.h"
#include "sim_types.h"
#include "simulator_operator.h"

#include <stdio.h>    /* printf  */
#include <stdlib.h>   /* rand    */
#include <stdint.h>   /* uint    */
#include <inttypes.h> /* PRId64  */
#include <stdbool.h>  /* bool    */

static ctrl_inf controller[2][MAX_POP];

static uint8_t same_controller(ctrl_inf *, ctrl_inf *);
static void    print_controller(ctrl_inf *);
static void    bubble_sort();
static void    combine_elite();
static void    combine_elite_general();
static void    combine_general();


void init_pop() {
    int i, j;
    bool     exists = false;
    ctrl_inf temp_controller;

    for (i = 0; i < MAX_POP - 1; i++) {
        do {
            temp_controller.kp   = ((float)rand()/(float)RAND_MAX) * 0.01;
            temp_controller.ki   = ((float)rand()/(float)RAND_MAX) * 0.00001;
            temp_controller.kd   = 0.00001;
            temp_controller.scr  = BAD_SCORE / 2.0;
            temp_controller.eval = false;

            exists = false;

            if (i > 1)
                for (j = 0; j <= (i-1); j++)
                    if (same_controller(&temp_controller, 
                                        &controller[0][j]) == 0)
                        exists = true;
        } while (exists == true);
        controller[0][i] = temp_controller;
    }
}


void combination() {
    combine_elite();
    combine_elite_general();
    combine_general();
}


void mutation() {

}


void run_ga() {
    uint64_t generation = 0;
    float    score;
    int      i;

    init_pop();

    while (1) {
        generation += 1;
        printf("Generation: %" PRIu64 "\n", generation);

        for (i = 0; i < MAX_POP; i++) {
            if (controller[0][i].eval == false) {
                score = carry_out_a_simulation(controller[0][i].kp,
                                               controller[0][i].ki,
                                               controller[0][i].kd);
                controller[0][i].scr  = score;
                controller[0][i].eval = true;
            }

            printf("%" PRIu64 " ", generation);
            print_controller(&controller[0][i]);
        }

        bubble_sort();


        if (controller[0][1].scr < 10.0 || generation > 50)
            break;

        /* prepare new generation in controller[1][x] */

        /* elitism 10% */
        for (i = 0; i < MAX_POP/10; i++) {
            controller[1][i] = controller[0][i];
            /* keep evaluated */
        }

        /* combination */
        combination();

        /* mutation */
        mutation();

        for (i = 0; i < MAX_POP; i++) {
            controller[0][i] = controller[1][i];
        }
    }

    printf("The best controller is:\n");
    printf("%f\n", controller[0][1].kp);
    printf("%f\n", controller[0][1].ki);
    printf("%f\n", controller[0][1].kd);
    printf("%f\n", controller[0][1].scr);

    printf("Generation: %" PRIu64 "\n", generation);
}


static uint8_t same_controller(ctrl_inf *c1, ctrl_inf *c2) {
    uint8_t res = 1;
    if ((c1->kp  == c2->kp) && (c1->ki  == c2->ki) && (c1->kd  == c2->kd)) {
        res = 0;
    }

    return res;
}

static void print_controller(ctrl_inf *ctrl) {
    printf("| %f | ", ctrl->kp);
    printf("%f | ", ctrl->ki);
    printf("%f | ", ctrl->kd);
    printf("%f |\n", ctrl->scr);
}

static void bubble_sort() {
    int      i;
    bool     changes;
    ctrl_inf temp_controller;

    changes = false;
    while (1) {
        for (i = 0; i < MAX_POP - 1; i++) {
            if (controller[0][i].scr > controller[0][i+1].scr) {
                changes = true;
                temp_controller    = controller[0][i];
                controller[0][i]   = controller[0][i+1];
                controller[0][i+1] = temp_controller;
            }
        }
        if (changes == false)
            break;
        changes = false;
    }

}

static void combine_elite() {
    return;
}

static void combine_elite_general() {
    return;
}

static void combine_general() {
    return;
}
