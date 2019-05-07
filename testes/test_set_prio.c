#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>

#include "../include/cthread.h"
#include "../include/cdata.h"

#include "../include/scheduler.h"
#include "../include/error.h"

csem_t *semaphore;

int current_thread_prio() {
    return scheduler_get_executing_thread()->prio;
}

void test_main_from_low_to_medium() {
    printf("Start: %s\n", __FUNCTION__);

    assert(current_thread_prio() == LOW_PRIO);

    assert(csetprio(34, MEDIUM_PRIO) == SUCCESS_CODE);

    assert(current_thread_prio() == MEDIUM_PRIO);

    printf("End: %s\n", __FUNCTION__);
    return;
}

void test_thread_from_high_to_low() {
    printf("Start: %s\n", __FUNCTION__);

    assert(current_thread_prio() == HIGH_PRIO);

    assert(csetprio(0, LOW_PRIO) == SUCCESS_CODE);

    csignal(semaphore);

    printf("End: %s\n", __FUNCTION__);
    return;
}

void test_main_still_medium() {
    printf("Start: %s\n", __FUNCTION__);

    assert(current_thread_prio() == MEDIUM_PRIO);

    printf("End: %s\n", __FUNCTION__);
    return;
}

int main(int argc, char **argv) {
    printf("Start main\n");

    test_main_from_low_to_medium();

    ccreate(test_thread_from_high_to_low, NULL, HIGH_PRIO);

    semaphore = malloc(sizeof(csem_t));
    csem_init(semaphore, 0);

    cwait(semaphore);

    test_main_still_medium();

    printf("End main\n");

    exit(0);
}
