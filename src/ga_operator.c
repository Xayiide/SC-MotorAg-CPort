#include <stdio.h>    /* printf      */
#include <stdlib.h>   /* rand, qsort */
#include <stdint.h>   /* uint        */
#include <stdbool.h>  /* bool        */
#include <inttypes.h> /* PRId64      */
#include <math.h>     /* floor       */

#include "ga_operator.h"
#include "simulator_operator.h"
#include "sim_types.h"
#include "test.h"

#define MAX_POP 10000
#define MAX_TRIES_COMB_MUTATION 10

struct ctrl_inf {
    float kp, ki, kd;
    float score;
    bool evaluated;
};

static struct ctrl_inf controller[2][MAX_POP];

static bool is_same_controller(struct ctrl_inf *a, struct ctrl_inf *b);
static void init_pop(struct ctrl_inf c[2][MAX_POP]);
static void mutation(struct ctrl_inf c[2][MAX_POP]);
static void combination(struct ctrl_inf c[2][MAX_POP]);
static void combine_elite(struct ctrl_inf c[2][MAX_POP]);
static void combine_elite_general(struct ctrl_inf c[2][MAX_POP]);
static void combine_general(struct ctrl_inf c[2][MAX_POP]);
static int  cmp_score(const void *left, const void *right);
static void sort(struct ctrl_inf c[2][MAX_POP]);
static double randf(void);
#if defined(PRINT_TRACES)
static void print_controller(struct ctrl_inf *c);
#endif

void gaop_run(const char *filename)
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

#if 0
#if defined(PRINT_TRACES)
            printf("%" PRIu64 " ", generation);
            print_controller(&controller[0][i]);
#endif
#endif
        }

        sort(controller);

#if defined(PRINT_TRACES)
            printf("%" PRIu64 " ", generation);
            print_controller(&controller[0][0]);
#endif

        /* Condición de éxito */
        if (controller[0][0].score < 10.0 || generation > 50) // TODO: sacar a constantes
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

        /* Copy controller[1][i] en controller[0][i] */
        for (i = 0; i < MAX_POP; i++) {
            controller[0][i] = controller[1][i];
        }
    }

    printf("The best controller is:\n");
    printf("kp: %f\n", controller[0][0].kp);
    printf("ki: %f\n", controller[0][0].ki);
    printf("kd: %f\n", controller[0][0].kd);
    printf("score: %f\n", controller[0][0].score);
    printf("Generation: %" PRIu64 "\n", generation);

    test_and_log_controller(controller[0][0].kp,
                    controller[0][0].ki,
                    controller[0][0].kd,
                    filename);
}


/* Static functions */
static bool is_same_controller(struct ctrl_inf *a, struct ctrl_inf *b)
{
    bool res = false;
    if ((a->kp == b->kp) && (a->ki == b->ki) && (a->kd == b->kd)) {
        res = true;
    }

    return res;
}

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

static void mutation(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    int y = 0;
    float param;
    float severity;
    float sign;

    struct ctrl_inf temp;
    bool exist;
    uint8_t num_tries = 0;

    for (i = (7*MAX_POP/10); i < MAX_POP; i++) {
        while (1) {
            /* Select an element */
            do {
                y = (int) floor(randf() * (MAX_POP));
            } while (y == 0);

            param    = randf(); /* Select a parameter to mutate */
            severity = randf(); /* Select a severity of mutation */
            sign     = randf(); /* Select the sign of the mutation */
            temp     = c[0][y];

            if (sign >= 0.5) {
                if (param < 0.3333) {
                    if (severity < 0.3333)
                        temp.kp = temp.kp + randf() * 0.001;
                    else if (severity < 0.6666)
                        temp.kp = temp.kp + randf() * 0.1;
                    else 
                        temp.kp = temp.kp + randf() * 10.0;
                }
                else if (param < 0.6666) {
                    if (severity < 0.3333)
                        temp.ki = temp.ki + randf() * 0.001;
                    else if (severity < 0.6666)
                        temp.ki = temp.ki + randf() * 0.1;
                    else
                        temp.ki = temp.ki + randf() * 10.0;
                }
                else {
                    if (severity < 0.3333)
                        temp.kd = temp.kd + randf() * 0.00000001;
                    else if (severity < 0.6666)
                        temp.kd = temp.kd + randf() * 0.0000001;
                    else
                        temp.kd = temp.kd + randf() * 0.0001;
                }
            }
            else {
                if (param < 0.3333) {
                    if (severity < 0.3333)
                        temp.kp = temp.kp - randf() * 0.001;
                    else if (severity < 0.6666)
                        temp.kp = temp.kp - randf() * 0.1;
                    else
                        temp.kp = temp.kp - randf() * 10.0;
                }
                else if (param < 0.6666) {
                    if (severity < 0.3333)
                        temp.ki = temp.ki - randf() * 0.001;
                    else if (severity < 0.6666)
                        temp.ki = temp.ki - randf() * 0.1;
                    else
                        temp.ki = temp.ki - randf() * 10.0;
                }
                else {
                    if (severity < 0.3333)
                        temp.kd = temp.kd - randf() * 0.00000001;
                    else if (severity < 0.6666)
                        temp.kd = temp.kd - randf() * 0.0000001;
                    else
                        temp.kd = temp.kd - randf() * 0.0001;
                }
            }

            if (temp.kp < 0.0)
                temp.kp = temp.kp * -1.0;

            if (temp.ki < 0.0)
                temp.ki = temp.ki * -1.0;

            if (temp.kd < 0.0)
                temp.kd = temp.kd * -1.0;

            temp.score = BAD_SCORE / 2;
            temp.evaluated = false;

            /* Verify if there exists a twin */
            exist = false;
            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp == temp.kp) &&
                    (c[1][j].ki == temp.ki) &&
                    (c[1][j].kd == temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false || num_tries >= MAX_TRIES_COMB_MUTATION)
                break;

            num_tries++;
        }

        c[1][i] = temp;
    }


}

static void combination(struct ctrl_inf c[2][MAX_POP])
{
    combine_elite(c);
    combine_elite_general(c);
    combine_general(c);
}

static void combine_elite(struct ctrl_inf c[2][MAX_POP])
{
    int i, j;
    int y = 0;
    int z = 0;

    float param;
    struct ctrl_inf temp;
    bool exist;
    uint8_t num_tries = 0;

    for (i = MAX_POP/10; i < 2*MAX_POP/10; i++) {
        while (1) {
            /* Select elements */
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y <= 0);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z == y) || (z <= 0));

            /* Select parameter of z element to combine with y parameters */
            param = randf();
            temp = c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][y].kd;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kd = c[0][y].kd;
            }
            else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][z].kd;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exist = false;
            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp == temp.kp) &&
                    (c[1][j].ki == temp.ki) &&
                    (c[1][j].kd == temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false || num_tries >= MAX_TRIES_COMB_MUTATION)
                break;

            num_tries++;
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
    uint8_t  num_tries = 0;

    for (i = 2*MAX_POP/10; i < 5*MAX_POP/10; i++) {
        while (1) {
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y == 0);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z == y) || (z <= 0));

            param = randf();
            temp= c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][y].kd;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kd = c[0][y].kd;
            } else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][z].kd;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exist = false;
            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp == temp.kp) &&
                    (c[1][j].ki == temp.ki) &&
                    (c[1][j].kd == temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false || num_tries >= MAX_TRIES_COMB_MUTATION)
                break;

            num_tries++;
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
    uint8_t  num_tries = 0;

    for (i = 5*MAX_POP/10; i < 7*MAX_POP/10; i++) {
        while (1) {
            do {
                y = (int) floor(randf() * (MAX_POP/10));
            } while (y == z);

            do {
                z = (int) floor(randf() * (MAX_POP/10));
            } while ((z == y) || (z <= 0));

            param = randf();
            temp = c[1][i];

            if (param < 0.3333) {
                temp.kp = c[0][z].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][y].kd;
            }
            else if (param < 0.6666) {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][z].ki;
                temp.kd = c[0][y].kd;
            } else {
                temp.kp = c[0][y].kp;
                temp.ki = c[0][y].ki;
                temp.kd = c[0][z].kd;
            }

            temp.score = BAD_SCORE / 2.0;
            temp.evaluated = false;

            exist = false;
            for (j = 1; j < (i-1); j++) {
                if ((c[1][j].kp == temp.kp) &&
                    (c[1][j].ki == temp.ki) &&
                    (c[1][j].kd == temp.kd)) {
                    exist = true;
                }
                if (exist == true)
                    break;
            }

            if (exist == false || num_tries >= MAX_TRIES_COMB_MUTATION)
                break;

            num_tries++;
        }

        c[1][i] = temp;
    }
}

static int  cmp_score(const void *left, const void *right)
{
    const struct ctrl_inf *a = left;
    const struct ctrl_inf *b = right;

    return(a->score > b->score) - (a->score < b->score);
}

static void sort(struct ctrl_inf c[2][MAX_POP])
{
    qsort(c[0], MAX_POP, sizeof(c[0][0]), cmp_score);
}

static double randf() {
    return (double)rand()/(double)RAND_MAX;
}

#if defined(PRINT_TRACES)
static void print_controller(struct ctrl_inf *c)
{
    printf("| kp: %f | ", c->kp);
    printf("ki: %f | ",   c->ki);
    printf("kd: %f | ",   c->kd);
    printf("score: %f |\n",  c->score);
}
#endif