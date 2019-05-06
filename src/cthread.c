#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"

// Other important constants
#define MAIN_TID 0


int id_count = 1;
int thread_main_already_created = 0;


/*
 * Handle the thread termination
 * is used as a callback for the makecontext function
 */
void *handle_termination() {

	printf("handle termination");
    if (scheduler_kill_thread_from_exec() != SUCCESS_CODE) {
		printf("\n\n\nThis shit failed");
		return;
	}
	printf("\n\n\ndid not failed\n");

	int resss = scheduler_schedule_next_thread();

	if (resss == SUCCESS_CODE) {
	printf("\n\n\n 1111 - did not failed \n");
	} else {
		printf("\n\n\n 1111 - did not failed \n");
	}

    return;
}

/*
 * Create a context
 */
 
int create_context(ucontext_t* context, ucontext_t* next) {

    if (getcontext(context) == FAILED) {
        return FAILED;
    }
    
    if (next == NULL) {
    	context->uc_link = 0;
    } else {
    	context->uc_link = next;
    }
    
    context->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    context->uc_stack.ss_size = SIGSTKSZ;

    return SUCCESS_CODE;
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
    main_thread->prio = LOW_PRIO; //cade um ->context = context


	int insertion_result = scheduler_insert_in_ready(main_thread);
    if (insertion_result != SUCCESS_CODE) return FAILED;
        
    thread_main_already_created = 1;
	return insertion_result;

}

/**
 * ccrete is responsible for creating a new threading and adding it to the ready queue
 * @param start: pointer for the function that will be executed
 * @param arg: arguments for the function
 * @param prio: thread priority
 * @return returns the thread id or failure code
 */
int ccreate (void* (*start)(void*), void *arg, int prio) {
    // first thing to do is to create the thread main if it is not created
	
    if(!thread_main_already_created) {
    	initialize_state_queues();
        initialize_main_thread();
    }
	
    // this is the structure that will have the thread block
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));

    tcb->tid = id_count++;
    tcb->state = PROCST_APTO;
    tcb->prio = prio;


    // this context will be used as callback
    ucontext_t *callback_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    create_context(callback_context, NULL);
    makecontext(callback_context, (void (*) (void)) handle_termination, 0); // when called, the callback_context will execute the handle_termination function


	// here is tricky, when the thread context is finished, it points to the current context (callback)
    if (create_context(&(tcb->context), callback_context)) return FAILED;     
    
    makecontext(&(tcb->context), (void (*) (void)) start, 1, arg);
    tcb->context.uc_link = callback_context;
    
    if (tcb->context.uc_link == NULL) return FAILED;

    // adding the tcb to the ready queue with priority verification
    int insertion_result = scheduler_insert_in_ready(tcb);
    if ( insertion_result != SUCCESS_CODE) return insertion_result;

	//scheduler_schedule_next_thread();
    
    return tcb->tid; 
	
}
  
int csem_init(csem_t *sem, int count) {

	if (sem == NULL) return NULL_POINTER;

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

    sem->count --;
    if (sem->count < 0) {

        return scheduler_block_thread(sem);

    }
    return SUCCESS_CODE;
}

int csignal(csem_t *sem) {

	if (sem == NULL) return NULL_POINTER;

    sem->count ++;
    if (sem->count <= 0) {

        return scheduler_free_thread(sem);

    }

    return SUCCESS_CODE;
}
