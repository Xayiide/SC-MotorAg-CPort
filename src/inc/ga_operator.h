#ifndef _GA_OPERATOR_H_
#define _GA_OPERATOR_H_

#include <stdbool.h>

#define MAX_POP 10000

typedef struct controller_inf {
    float kp, ki, kd;
    float scr;   /* score     */
    bool  eval;  /* evaluated */
} ctrl_inf;

void init_pop();
void combination();
void mutation();

void run_ga();

#endif
