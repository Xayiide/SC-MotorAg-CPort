#include "ga_operator.h"
#include "sim_types.h"
#include "simulator_operator.h"

#include <stdio.h>    /* printf  */
#include <stdlib.h>   /* rand    */
#include <stdint.h>   /* uint    */
#include <stdbool.h>  /* bool    */
#include <inttypes.h> /* PRId64  */
#include <math.h>     /* floor   */

#define MAX_POP 10000

struct ctrl_inf {
    float kp, ki, kd;
    float score;
    bool evaluated;
};

static struct ctrl_inf controller[2][MAX_POP];

static void init_pop(struct ctrl_inf c[2][MAX_POP]);
static void combination(struct ctrl_inf c[2][MAX_POP]);
static void mutation(struct ctrl_inf c[2][MAX_POP]);
static bool is_same_controller(struct ctrl_inf *a, struct ctrl_inf *b);
static void print_controller(struct ctrl_inf *c);
static void bubble_sort(struct ctrl_inf c[2][MAX_POP]);
static void combine_elite(struct ctrl_inf c[2][MAX_POP]);
static void combine_elite_general(struct ctrl_inf c[2][MAX_POP]);
static void combine_general(struct ctrl_inf c[2][MAX_POP]);
static double randf(void);

void gaop_run(void)
{
    uint64_t generation = 0;
    float    score;
    int      i;

    init_pop(controller);

    while (1) {
        generation += 1;
        printf("Generation: %" PRIu64 "\n", generation);

        for (i = 0; i < MAX_POP; i++) {
            if (controller[0][i].evaluated == false) {
                score = simop_simulate(controller[0][i].kp,
                                       controller[0][i].ki,
                                       controller[0][i].kd);
                controller[0][i].score= score;
                controller[0][i].evaluated = true;
            }

            printf("%" PRIu64 " ", generation);
            print_controller(&controller[0][i]);
        }

        bubble_sort(controller);


        if (controller[0][1].score < 10.0 || generation > 50)
            break;

        /* prepare new generation in controller[1][x] */

        /* elitism 10% */
        for (i = 0; i < MAX_POP/10; i++) {
            controller[1][i] = controller[0][i];
            /* keep evaluated */
        }

        /* combination */
        combination(controller);

        /* mutation */
        mutation(controller);

        for (i = 0; i < MAX_POP; i++) {
            controller[0][i] = controller[1][i];
        }
    }

    printf("The best controller is:\n");
    printf("%f\n", controller[0][1].kp);
    printf("%f\n", controller[0][1].ki);
    printf("%f\n", controller[0][1].kd);
    printf("%f\n", controller[0][1].score);

    printf("Generation: %" PRIu64 "\n", generation);
}


/* Static functions */

static void init_pop(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    bool exists = false;
    struct ctrl_inf temp;

    for (i = 0; i < MAX_POP; i++) {
        do {
            temp.kp = ((float)rand()/(float)RAND_MAX) * 0.01;
            temp.ki = ((float)rand()/(float)RAND_MAX) * 0.00001;
            temp.kd = 0.00001;
            temp.score  = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exists = false;

            if (i > 1)
                for (j = 0; j <= (i-1); j++)
                    if (is_same_controller(&temp, &c[0][j]))
                        exists = true;
        } while (exists == true);
        c[0][i] = temp;
    }
}

static void combination(struct ctrl_inf c[2][MAX_POP])
{
    combine_elite(c);
    combine_elite_general(c);
    combine_general(c);
}

static void mutation(struct ctrl_inf c[2][MAX_POP])
{
    (void) c;
}

static bool is_same_controller(struct ctrl_inf *a, struct ctrl_inf *b)
{
    bool res = false;
    if ((a->kp == b->kp) && (a->ki == b->ki) && (a->kd == b->kd)) {
        res = true;
    }

    return res;
}

static void print_controller(struct ctrl_inf *c)
{
    printf("| %f | ", c->kp);
    printf("%f | ",   c->ki);
    printf("%f | ",   c->kd);
    printf("%f |\n",  c->score);
}

static void bubble_sort(struct ctrl_inf c[2][MAX_POP])
{
    int  i;
    bool changes;
    struct ctrl_inf temp;

    changes = false;
    while (1) {
        for (i = 0; i < MAX_POP - 1; i++) {
            if (c[0][i].score > c[0][i+1].score) {
                changes = true;
                temp = c[0][i];
                c[0][i] = c[0][i+1];
                c[0][i+1] = temp;
            }
        }
        if (changes == false)
            break;
        changes = false;
    }

}

static void combine_elite(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    int y = 0;
    int z = 0;

    float param;
    struct ctrl_inf temp;
    bool exist;

    for (i = MAX_POP/10 + 1; i < 2*MAX_POP/10; i++) {
        while (1) {
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y <= 0);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z <= 0) || (z == y));

            param = randf();
            temp = c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][y].kp;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kp = c[0][y].kp;
            }
            else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][z].kp;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;
            
            exist = false;

            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp = temp.kp) &&
                    (c[1][j].ki = temp.ki) &&
                    (c[1][j].kd = temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false)
                break;
        }

        c[1][i] = temp;
    }
}

static void combine_elite_general(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    int y = 0;
    int z = 0;

    float    param;
    struct ctrl_inf temp;
    bool     exist;

    for (i = 2*MAX_POP/10 + 1; i < 5*MAX_POP/10; i++) {
        while (1) {
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y == 0);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z <= 0) || (z == y));

            param = randf();
            temp= c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][y].kp;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kp = c[0][y].kp;
            } else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][z].kp;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exist = false;

            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp = temp.kp) &&
                    (c[1][j].ki = temp.ki) &&
                    (c[1][j].kd = temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false)
                break;
        }

        c[1][i] = temp;
    }
}

static void combine_general(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    int y = 0;
    int z = 0;

    float    param;
    struct ctrl_inf temp;
    bool     exist;

    for (i = 5*MAX_POP/10 + 1; i < 7*MAX_POP/10; i++) {
        while (1) {
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y == z);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z <= 0) || (z == y));

            param = randf();
            temp = c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][y].kp;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kp = c[0][y].kp;
            } else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kp = c[0][z].kp;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exist = false;

            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp = temp.kp) &&
                    (c[1][j].ki = temp.ki) &&
                    (c[1][j].kd = temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false)
                break;
        }

        c[1][i] = temp;
    }
}

static double randf() {
    return (double)rand()/(double)RAND_MAX;
}
