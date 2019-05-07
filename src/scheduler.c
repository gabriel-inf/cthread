#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

//executing, ready_low, ready_medium, ready_high
int initialize_state_queues(){

	ready_low = malloc(sizeof(PFILA2));
	ready_medium = malloc(sizeof(PFILA2));
	ready_high = malloc(sizeof(PFILA2));
	executing = malloc(sizeof(PFILA2));
	
	if (ready_low == NULL) return MALLOC_ERROR;
	if (ready_medium == NULL) return MALLOC_ERROR;
	if (ready_high == NULL) return MALLOC_ERROR;
	if (executing == NULL) return MALLOC_ERROR;

	if (CreateFila2(ready_low) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(ready_medium) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(ready_high) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

	return SUCCESS_CODE;
}


int scheduler_block_thread(csem_t *sem) {

	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;
	
	TCB_t *executing_thread = scheduler_get_executing_thread();
	if (executing_thread == NULL) return NULL_POINTER;

	executing_thread->state = PROCST_BLOQ;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;		
	if (AppendFila2(sem->fila, (void *)executing_thread)) return LINE_OPERATION_ERROR;
	
	if (getcontext(&(executing_thread->context)) == FAILED) return FAILED;
	return scheduler_schedule_next_thread();

	//TCB_t *next = malloc(sizeof(TCB_t));
	//if (scheduler_get_first_ready_thread( &next ) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	
	//if (AppendFila2(executing, (void *)next)) return LINE_OPERATION_ERROR;
	
	
	//if (swapcontext(&(executing_thread->context), &(next->context) ) != SUCCESS_CODE) return CONTEXT_ERROR;
	//printf("swap finished\n");

	//return SUCCESS_CODE;

}

int scheduler_free_thread(csem_t *sem) {

	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;
	
	if (FirstFila2(sem->fila) != SUCCESS_CODE) return EMPTY_LINE;
	TCB_t *thread_to_wake = sem->fila->first->node;
	thread_to_wake->state = PROCST_APTO;
	
	if (DeleteAtIteratorFila2(sem->fila) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	return scheduler_insert_in_ready(thread_to_wake);
	
}

/**
 * If there is a thread to be executed, it will set the context and deal with the queues
 * @return: return SUCCESS_CODE when it gets a new thread to be executed otherwise an error
 * next will have the thread to be executed 
 */

int scheduler_get_first_ready_thread( TCB_t** next ) {
	
	if ( *next == NULL ) return NULL_POINTER;
	
	if ( ( ready_high != NULL ) && (FirstFila2(ready_high) == SUCCESS_CODE) ) {
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_high);
		
		return SUCCESS_CODE;
	
	} else if ((ready_medium != NULL) && ( FirstFila2(ready_medium) == SUCCESS_CODE )) { 
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_medium);
		return SUCCESS_CODE;
	
	} else if ((ready_low != NULL) && ( FirstFila2(ready_low) == SUCCESS_CODE )) {
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_low); //mudar pra func do sor
		return SUCCESS_CODE;
	
	} else {
		
		return NOTHING_TO_SCHEDULE;
	
	}
	
}

int scheduler_schedule_next_thread() {

    if (executing == NULL) return EMPTY_LINE;
    
   	TCB_t *next = malloc(sizeof(TCB_t));
   	if (next == NULL) return MALLOC_ERROR;
    
    int next_result = scheduler_get_first_ready_thread(&next);
    if (next_result != SUCCESS_CODE) return next_result;
	
	int prio = next->prio;
	
	switch (prio) {
        case HIGH_PRIO:
        	
        	if (FirstFila2(ready_high) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_high) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	break;
        	
        case MEDIUM_PRIO:
        	
        	if (FirstFila2(ready_medium) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_medium) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	break;
        	
        case LOW_PRIO:
        
            if (FirstFila2(ready_low) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_low) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
            break;
            
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
    
    next->state = PROCST_EXEC;

    if (AppendFila2(executing, (void *) next) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (&(next->context) == NULL) return FAILED;

    if ( next->context.uc_stack.ss_sp == NULL) return NULL_POINTER;
	
	if (DEBUG) {
	
		show_state_queues();
		printf("\n - The thread that will be executed: %d\n", next->tid);
	
	}
	
    return setcontext(&(next->context));
}

int scheduler_kill_thread_from_exec() {

	TCB_t *executing_thread = scheduler_get_executing_thread();
	if (executing_thread == NULL) return NULL_POINTER;
	
	executing_thread->state = PROCST_TERMINO;
	

	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

	free(executing_thread); //Laura does not have sure

	
	return SUCCESS_CODE;
	
}

int send_exec_to_ready() {

	if (executing == NULL) return NULL_POINTER;
	if (FirstFila2(executing) != SUCCESS_CODE) return EMPTY_LINE;

	TCB_t *executing_thread = (TCB_t *) GetAtIteratorFila2(executing);
	
	executing_thread->state = PROCST_APTO;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

	int insert_ready_result = scheduler_insert_in_ready(&executing_thread);
	if (insert_ready_result != SUCCESS_CODE) return insert_ready_result;	
	show_state_queues();
	printf("\nTid=%d, Tprio=%d\n", executing_thread->tid, executing_thread->prio);
	return scheduler_insert_in_ready(&executing_thread);
	
}


int scheduler_insert_in_ready(TCB_t *thread) {

	int prio = thread->prio;

	switch (prio) {
        case HIGH_PRIO:
        	return AppendFila2(ready_high, (void *) thread);
        case MEDIUM_PRIO:
        	return AppendFila2(ready_medium, (void *) thread);
        case LOW_PRIO:
            return AppendFila2(ready_low, (void *) thread);
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }

}

TCB_t* scheduler_get_executing_thread() {
    if (executing == NULL) return NULL;
    if (FirstFila2(executing) != SUCCESS_CODE) return NULL;

    return (TCB_t *) GetAtIteratorFila2(executing);
}

void scheduler_show_state_queues() {

	printf("---------------------READY QUEUES:-------------------\n");

	if (FirstFila2(ready_low) != SUCCESS_CODE) printf("- READY_LOW IS EMPTY!\n");
	else {
		printf("Ready low:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_low);	
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_low) == SUCCESS_CODE);
	}
	
	if (FirstFila2(ready_medium) != SUCCESS_CODE) printf("- READY_MEDIUM IS EMPTY!\n");
	else {
		printf("Ready medium:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_medium);	
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_medium) == SUCCESS_CODE);
	}
	
	if (FirstFila2(ready_high) != SUCCESS_CODE) printf("- READY_HIGH IS EMPTY!\n");
	else {
		printf("Ready high:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_high);	
			printf("	-> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_high) == SUCCESS_CODE);
	}

	printf("---------------------EXEC:-------------------\n");

	if (FirstFila2(executing) != SUCCESS_CODE) printf("- executing is empty\n");
	else {
		printf("executing:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(executing);	
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_high) == SUCCESS_CODE);
	}
}
