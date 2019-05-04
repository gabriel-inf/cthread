#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"


// Priorities
#define LOW_PRIO 0
#define MEDIUM_PRIO 1
#define HIGH_PRIO 2

// Errors
#define FAILED -1
#define ERROR_PRIO_NOT_DEFINED -2
#define NOTHING_TO_SCHEDULE -3
#define SUCCESS 0

// Other important constants
#define MAIN_TID 0

PFILA2 ready, executing, blocked;

int id_count = 1;
int thread_main_already_created = 0;

/**
 * Selects the thread that is running and preempt it
 * @return the status of failure or success
 */
int preempt_running_thread() {
    if (FirstFila2(executing) != 0) {
        return SUCCESS;
    } else {
        TCB_t *executing_thread = GetAtAntIteratorFila2(executing);
        //TODO: unblock the thread that was waiting for the semaphore
        // free_blocked_thread(executing_thread->tid) // blocked by join (Rodrigo)
        // depends on unlocked threads
        remove_thread_from_queue(executing, executing_thread->id);
        free(executing_thread);
        return SUCCESS;
    }
}

/**
 * If there is a thread to be executed, it will set the context and deal with the queues
 * @return: return if everything was ok
 */
int schedule_next_thread() {
    TCB_t *thread = select_next_thread();

    if (thread != NULL) {
        remove_thread_from_queue(ready); // generalize for one queue
        AppendFila2(executing, thread);
        setcontext(&(thread->context));
        return SUCCESS;
    } else {
        return NOTHING_TO_SCHEDULE;
    }
}

/*
 * Handle the thread termination
 * is used as a callback for the makecontext function
 */
int handle_termination() {
    preempt_running_thread();
    return schedule_next_thread();
}

/*
 * Create a context
 */
int create_context(ucontext_t* context, ucontext_t* next) {

    if(getcontext(context) == FAILED) {
        return FAILED;
    }

    context->uc_link = next;
    context->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    context->uc_stack.ss_size = SIGSTKSZ;

    return SUCCESS;
}

/*
 *  Initializes the main thread
 *  returns 0 if succeeded and -1 if failed
 */
int initialize_main_thread() {

    ucontext_t *current_context = (ucontext_t*) malloc(sizeof(ucontext_t));

    create_context(current_context, NULL);

    TCB_t *main_thread = malloc(sizeof(TCB_t));
    main_thread->tid = MAIN_TID;
    main_thread->prio = LOW_PRIO;

    if (AppendFila2(ready, main_thread) == SUCCESS) {
        thread_main_already_created = 1;
        return SUCCESS;
    } else {
        return FAILED;
    }
}

/**
 * ccrete is responsible for creating a new threading and adding it to the ready queue
 * @param start: pointer for the function that will be executed
 * @param arg: arguments for the function
 * @param prio: thread priority
 * @return returns a code of SUCCESS or failure
 */
int ccreate (void* (*start)(void*), void *arg, int prio) {

    // first thing to do is to create the thread main if it is not created
    if(!thread_main_already_created) {
        initialize_main_thread();
    }
    // this is the struct that will have the thread block
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));

    tcb->tid = id_count++;
    tcb->state = PROCST_APTO;
    tcb->prio = prio;

    // this context will be used as callback
    ucontext_t *current_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    create_context(current_context, NULL);
    makecontext(current_context, (void (*) (void)) handle_termination, 0); // when called, the current_context will execute the handle_termination function

    create_context(&(tcb->context), current_context); // here is tricky, when the thread context is finished, it points to the current context (callback)
    makecontext(&(tcb->context), (void (*) (void)) start, 1, arg);

    // adding the tcb to the ready queue with priority verification
    switch (prio){
        case HIGH_PRIO:
        case MEDIUM_PRIO:
        case LOW_PRIO:
            AppendFila2(ready, tcb);
            break;
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
    return tcb->tid;
}
  
int csem_init(csem_t *sem, int count) {

    PFILA2 pfila = malloc(sizeof(FILA2));

    sem->count = count;
    sem->fila = pfila;

    if (CreateFila2(pfila) == SUCCESS_CODE) {
        return SUCCESS_CODE;
    } else {
        return ERROR_CODE;
    }
}

int cwait(csem_t *sem) {

    sem->count --;
    if (sem->count < 0) {

        if (executing == NULL) return ERROR_CODE;
        if (sem->fila == NULL) return ERROR_CODE;

        if ( FirstFila2(executing) != SUCCESS_CODE ) return ERROR_CODE;
        TCB_t *executing_process = (TCB_t *)executing->first->node;
        executing_process->state = PROCST_BLOQ;

        if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return ERROR_CODE;
        if (AppendFila2(sem->fila, &executing_process) != SUCCESS_CODE) return ERROR_CODE;

    }
    return SUCCESS_CODE;
}

int csignal(csem_t *sem) {

    sem->count ++;
    if (sem->count <= 0) {

        if (sem->fila == NULL) return ERROR_CODE;
        if (ready == NULL) return ERROR_CODE;

        TCB_t *process_to_wake = (TCB_t *) sem->fila->first->node;
        process_to_wake->state = PROCST_APTO;
        if (FirstFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
        if (DeleteAtIteratorFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
        if (AppendFila2(ready, &process_to_wake) != SUCCESS_CODE) return ERROR_CODE;

    }

    return SUCCESS_CODE;
}
