#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

int scheduler_block_thread(csem_t *sem) {


	if (executing == NULL) return NULL_POINTER;
	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;
	
	if (FirstFila2(executing) != SUCCESS_CODE) return EMPTY_LINE;
	
	TCB_t *executing_thread = (TCB_t *) executing->first->node;
	executing_thread->state = PROCST_BLOQ;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;		
	if (AppendFila2(sem->fila, (void *)executing_thread)) return LINE_OPERATION_ERROR;
	
	//if (getcontext(&(executing_thread->context)) == FAILED) return FAILED;
	printf("swap started\n");
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
	
		*next = (TCB_t *) ready_high->first->node;
		return SUCCESS_CODE;
	
	} else if ((ready_medium != NULL) && ( FirstFila2(ready_medium) == SUCCESS_CODE )) { 
	
		*next = (TCB_t *) ready_medium->first->node;
		return SUCCESS_CODE;
	
	} else if ((ready_low != NULL) && ( FirstFila2(ready_low) == SUCCESS_CODE )) {
	
		*next = (TCB_t *) ready_low->first->node;
		return SUCCESS_CODE;
	
	} else {
		
		return NOTHING_TO_SCHEDULE;
	
	}
	
}

/**
 * If there is a thread to be executed, it will set the context and deal with the queues
 * @return: return if everything was ok
 */
 
int scheduler_schedule_next_thread() {
    
    if (executing == NULL) return EMPTY_LINE;
    
   	TCB_t *next = malloc(sizeof(TCB_t));
   	if (next == NULL) return MALLOC_ERROR;
    
    int next_result = scheduler_get_first_ready_thread(&next);
    if (next_result != SUCCESS_CODE) return next_result;
	
	int prio = next->prio;
	printf("%d\n", prio);
	printf("tiddsnkdsnkdsdkn = %d\n", next->tid);
	
	switch (prio) {
        case HIGH_PRIO:
        	
        	if (FirstFila2(ready_high) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_high) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	break;
        case MEDIUM_PRIO:
        	printf("laura =\n");
        	if (FirstFila2(ready_medium) != SUCCESS_CODE) return EMPTY_LINE;
        	printf("laura =\n");
        	if (DeleteAtIteratorFila2(ready_medium) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	printf("laura =\n");
        	break;
        case LOW_PRIO:
            if (FirstFila2(ready_low) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_low) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
            break;
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
    printf("laura =\n");
    if (FirstFila2(executing) == SUCCESS_CODE) return EMPTY_LINE;
    printf("eu =\n");
    next->state = PROCST_EXEC;
    if (AppendFila2(executing, (void *) next) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    printf("soccorro =\n");
    if (&(next->context) == NULL) return FAILED;
    ucontext_t aa;
    printf("%d\n", next->context.uc_stack.ss_size);
    if ( next->context.uc_stack.ss_sp == NULL) return NULL_POINTER; //tem que tratar o error 
    
    printf("vamo\n");
    return swapcontext(&aa, &(next->context));
    //printf("%d", setcontext(&(next->context)) );
    //if (== FAILED) return FAILED;
    //printf("alooooo\n");
    //return SUCCESS_CODE;
    
    // TENHO UMA DUVIDA NESSA: ao inves de chamarmos setcontext, nao deveriamos chamar swapcontext (interrogacao) tipo,
    // nao deveriamos salvar o atual em algum lugar (int)
    // o set context nem retorna se deu sucesso e retorna -1 se deu erro, 
    // entao faz sentido colocar um return success_code apos sua chamada (interrogacao)
   
}

/*
int scheduler_remove_thread_from_exec() {

	if (executing == NULL) return NULL_POINTER;
	if (FirstFila2(executing) != SUCCESS_CODE) return EMPTY_LINE;

	TCB_t *executing_thread = (TCB_t *)executing->first->node;
	
	executing_thread->state = 
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
	
	return getcontext(&(executing_thread->context);
	
}*/

int scheduler_insert_in_ready(TCB_t *thread) {
	
	int prio = thread->prio;
	
	switch (prio) {
        case HIGH_PRIO:
        	if (ready_high == NULL) return NULL_POINTER;
        	return AppendFila2(ready_high, (void *) thread);
        case MEDIUM_PRIO:
        	if (ready_medium == NULL) return NULL_POINTER;
        	return AppendFila2(ready_medium, (void *) thread);
        case LOW_PRIO:
        	if (ready_low == NULL) return NULL_POINTER;
            return AppendFila2(ready_low, (void *) thread);
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }

}
