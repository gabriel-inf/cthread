#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>

#include "../include/cthread.h"
#include "../include/cdata.h"

#include "../include/scheduler.h"
#include "../include/error.h"

void test_join_from_main(void *step) {
    int *internal_step = (int *) step;

    assert(*internal_step == 1);

    *internal_step = 2;
}

int main(int argc, char **argv) {
    printf("Start main\n");

    int step;
    step = 0;

    int tid = ccreate((void *(*)(void *)) test_join_from_main, &step, MEDIUM_PRIO);
    assert(tid > 0);

    assert(step == 0);

    step = 1;

    assert(cjoin(tid) == SUCCESS_CODE);

    assert(step == 2);

    printf("End main\n");

    exit(0);
}