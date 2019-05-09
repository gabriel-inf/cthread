#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>

#include "../include/cthread.h"
#include "../include/cdata.h"

#include "../include/scheduler.h"
#include "../include/error.h"

typedef struct s_test_parameters {
    int tid;
    int	*step;
} TP_t;

void test_join_from_main(void *step) {
    printf("Start: %s\n", __FUNCTION__);

    // Just to not have to cast multiple times
    int *internal_step = (int *) step;

    assert(*internal_step == 1);
    *internal_step = 2;

    printf("End: %s\n", __FUNCTION__);
}

void *test_everyone_blocked(int *step) {
    printf("Start: %s\n", __FUNCTION__);

    assert(*step == 8);
    *step = 9;

    // Booth would be blocked if this succeed
    assert(cjoin(0) == NOTHING_TO_SCHEDULE);

    assert(*step == 9);
    *step = 10;

    printf("End: %s\n", __FUNCTION__);
    return NULL;
}

void test_multiple_joins(TP_t *params) {
    printf("Start: %s\n", __FUNCTION__);

    assert(params->tid > 0);
    assert(*params->step == 6);
    *params->step = 7;

    // Trying to join on "deadlock" too, but only one thread can be waiting any specific thread
    assert(cjoin(params->tid) == ALREADY_JOINED_ERROR);

    assert(*params->step == 7);
    *params->step = 8;

    printf("End: %s\n", __FUNCTION__);
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
    step = 3;

    // Cannot cjoin on thread that does not exit or has finished
    assert(cjoin(tid) == INVALID_THREAD);

    assert(step == 3);
    step = 4;

    // The thread cannot join itself
    assert(cjoin(0) == INVALID_THREAD);

    assert(step == 4);
    step = 5;

    int deadlock = ccreate((void *(*)(void *)) test_everyone_blocked, &step, LOW_PRIO);
    assert(deadlock > 0);

    TP_t parameters;
    parameters.tid = deadlock;
    parameters.step = &step;

    int invalid = ccreate((void *(*)(void *)) test_multiple_joins, &parameters, HIGH_PRIO);
    assert(invalid > 0);

    assert(step == 5);
    step = 6;

    cjoin(deadlock);

    assert(step == 10);

    printf("End main\n");

    exit(0);
}
