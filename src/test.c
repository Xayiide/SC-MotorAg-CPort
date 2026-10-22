#include <stdio.h> /* printf, fopen */
#include <stddef.h> /* NULL */

#include "test.h"
#include <dc_motor_sim.h>
#include "controller.h"


void test_and_log_controller(float kp, float ki, float kd, const char *fn)
{
    float speed;
    struct dcmsim_info dcmotor;
    struct controller ctrl;

    int i;
    FILE *fd;

    fd = fopen(fn, "w");
    if (fd == NULL) {
        printf("Error abriendo archivo: %s\n", fn);
        return;
    }

    dcmsim_init(&dcmotor);
    ctrl_init(&ctrl, kp, ki, kd);
    ctrl_set_reference(&ctrl, 80.0);

    ctrl_exec_cycle(&ctrl, &dcmotor);
    dcmsim_exec_sim_cycle(&dcmotor);
    for (i = 0; i < 2000; i++) {
        speed = dcmsim_get_speed(&dcmotor);
        if (i % 10 == 0)
            ctrl_exec_cycle(&ctrl, &dcmotor);
        dcmsim_exec_sim_cycle(&dcmotor);
        fprintf(fd, "%f\n", speed);
    }

    fclose(fd);
    printf("Prueba registrada correctamente en %s\n", fn);
}