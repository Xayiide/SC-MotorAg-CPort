#ifndef _GA_OPERATOR_H_
#define _GA_OPERATOR_H_

#include <stdbool.h>

#define MAX_POP 10000

typedef struct controller_inf {
    float Kp, Ki, Kd;
    float score;
    bool  evaluated;
} controller_inf;



void run_ga();

#endif
