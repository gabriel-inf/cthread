#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"

#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"

// 128 MB
#define STACK_SIZE 131072

// Priorities
#define LOW_PRIO 0
#define MEDIUM_PRIO 1
#define HIGH_PRIO 2

#define MAIN_TID 0

#define ERROR_PRIO_NOT_DEFINED -1
#define  FAILED -1
#define SUCCESS 0

FILA2 ready_low, ready_medium, ready_high, executing, blocked;

ucontext_t *final_context = NULL;
TCB_t *main_tcb = NULL;
int id_count = 1;
int thread_main_already_created = 0;

/*
 * Create a context
 */
int create_context(ucontext_t* context, ucontext_t* next) {

    if(getcontext(context) == FAILED) {
        return FAILED;
    }

    context->uc_link = next;
    context->uc_stack.ss_sp = (char*) malloc(STACK_SIZE);
    context->uc_stack.ss_size = STACK_SIZE;

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

    if (AppendFila2(ready_low) == SUCCESS) {
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

    if(!thread_main_already_created) {
        initialize_main_thread();
    }

    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));

    tcb->tid = id_count++;
    tcb->state = PROCST_APTO;
    tcb->prio = prio;

    ucontext_t *current_context = (ucontext_t*) malloc(sizeof(ucontext_t));

    create_context(current_context, NULL);
    create_context(&(tcb->context), current_context);

    makecontext(current_context, (void (*) (void)) CB_end_thread, 0);
    makecontext(&(tcb->context), (void (*) (void)) start, 1, arg);

    switch (prio){
        case HIGH_PRIO:
            AppendFila2(ready_high, tcb);
            break;
        case MEDIUM_PRIO:
            AppendFila2(ready_medium, tcb);
            break;
        case LOW_PRIO:
            AppendFila2(ready_low, tcb);
            break;
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
    return tcb->tid;
}