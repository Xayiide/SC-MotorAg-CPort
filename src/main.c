#include <stdio.h>  /* NULL  */
#include <stdlib.h> /* srand */
#include <time.h>   /* time  */
#include "ga_operator.h"

int main() {
    srand(time(NULL));

    run_ga();

    return 0;
}
