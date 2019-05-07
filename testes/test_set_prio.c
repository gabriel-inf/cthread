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
    printf(__FUNCTION__);

    assert(current_thread_prio() == LOW_PRIO);

    assert(csetprio(34, MEDIUM_PRIO) == SUCCESS_CODE);

    assert(current_thread_prio() == MEDIUM_PRIO);

    return;
}

void test_thread_from_high_to_low() {
    printf(__FUNCTION__);

    assert(current_thread_prio() == HIGH_PRIO);

    assert(csetprio(NULL, LOW_PRIO) == SUCCESS_CODE);

    csignal(semaphore);

    return;
}

void test_main_still_medium() {
    printf(__FUNCTION__);

    assert(current_thread_prio() == MEDIUM_PRIO);

    return;
}

int main(int argc, char **argv) {
    printf("Start");

    test_main_from_low_to_medium();

    ccreate(test_thread_from_high_to_low, NULL, HIGH_PRIO);

    semaphore = malloc(sizeof(csem_t));
    csem_init(semaphore, 0);

    cwait(semaphore);

    test_main_still_medium();

    printf("End");

    exit(0);
}