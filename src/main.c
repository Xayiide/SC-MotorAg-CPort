#include <stdio.h>  /* NULL  */
#include <stdlib.h> /* srand */
#include <time.h>   /* time  */

#include "ga_operator.h"

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc != 2) {
        printf("Uso: %s <test_log_file>\n", argv[0]);
        return 1;
    }

    gaop_run(argv[1]);

    return 0;
}
