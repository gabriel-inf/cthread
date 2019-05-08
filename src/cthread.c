#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"

int id_count = 1;

/**
 * Handle the thread termination
 * is used as a callback for the makecontext function
 */
void *handle_termination() {
    if (DEBUG) printf("Start: %s\n", __FUNCTION__);

    int first_result = scheduler_kill_thread_from_exec();
    if (DEBUG) printf("First result: %d", first_result);

    if (first_result != SUCCESS_CODE) return NULL;

	int second_result = scheduler_schedule_next_thread();
    if (DEBUG) printf("Second result: %d", second_result);

    if (DEBUG) printf("End: %s\n", __FUNCTION__);

    return NULL;
}

int validate_prio(int prio) {
    if (prio == HIGH_PRIO || prio == MEDIUM_PRIO || prio == LOW_PRIO) {
        return SUCCESS_CODE;

    } else {
        return INVALID_PRIO;
    }
}

/**
 * ccreate is responsible for creating a new threading and adding it to the ready queue
 * @param start: pointer for the function that will be executed
 * @param arg: arguments for the function
 * @param prio: thread priority
 * @return returns the thread id or failure code
 */
int ccreate (void* (*start)(void*), void *arg, int prio) {
    if (validate_prio(prio) != SUCCESS_CODE) return INVALID_PRIO;

    // First thing to do is to create the thread main if it is not created
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

	
    // this is the structure that will have the thread block
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));

    tcb->tid = id_count++;
    tcb->state = PROCST_APTO;
    tcb->prio = prio;


    // this context will be used as callback
    ucontext_t *callback_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    scheduler_create_context(callback_context, NULL);
    makecontext(callback_context, (void (*) (void)) handle_termination, 0); // when called, the callback_context will execute the handle_termination function


	// here is tricky, when the thread context is finished, it points to the current context (callback)
    if (scheduler_create_context(&(tcb->context), callback_context) != SUCCESS_CODE) return FAILED;
    
    makecontext(&(tcb->context), (void (*) (void)) start, 1, arg);
    tcb->context.uc_link = callback_context;
    
    if (tcb->context.uc_link == NULL) return FAILED;

    // adding the TCB to the ready queue with priority verification
    int insertion_result = scheduler_insert_in_ready(tcb);
    if ( insertion_result != SUCCESS_CODE) return insertion_result;
    
    return tcb->tid;
}

int cyield(void) {
    // First thing to do is to create the thread main if it is not created

	TCB_t *exec_tcb, *next_tcb;
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

	int state_migration_result = scheduler_send_exec_to_ready();
	if (state_migration_result != SUCCESS_CODE) return state_migration_result;
	printf("cyeld end");
	return scheduler_schedule_next_thread();

}

  
int csem_init(csem_t *sem, int count) {
    if (sem == NULL) return NULL_POINTER;

    // First thing to do is to create the thread main if it is not created
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

    PFILA2 pfila = malloc(sizeof(FILA2));
   	if (pfila == NULL) return MALLOC_ERROR;

    sem->count = count;
    sem->fila = pfila;

    if (CreateFila2(pfila) == SUCCESS_CODE) {
        return SUCCESS_CODE;
    } else {
        return LINE_OPERATION_ERROR;
    }
}

int cwait(csem_t *sem) {
    if (sem == NULL) return NULL_POINTER;

    // First thing to do is to create the thread main if it is not created
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

    sem->count --;
    if (sem->count < 0) {

        return scheduler_block_thread(sem);

    }
    return SUCCESS_CODE;
}

int csignal(csem_t *sem) {
    if (sem == NULL) return NULL_POINTER;

    // First thing to do is to create the thread main if it is not created
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

	if (FirstFila2(sem->fila) != SUCCESS_CODE) sem->count ++;

    if (sem->count <= 0) {
        return scheduler_free_thread(sem);
    }
	printf("hey\n");

    return SUCCESS_CODE;
}

int csetprio(int tid, int prio) {
    if (validate_prio(prio) != SUCCESS_CODE) return INVALID_PRIO;

    // First thing to do is to create the thread main if it is not created
    int init_result = scheduler_init();
    if (init_result != SUCCESS_CODE) return init_result;

    TCB_t *executing_thread = scheduler_get_executing_thread();
    if (executing_thread == NULL) return NULL_POINTER;

    executing_thread->prio = prio;

    return SUCCESS_CODE;
}
