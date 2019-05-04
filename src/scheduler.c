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
	
	
	TCB_t *next = malloc(sizeof(TCB_t));
	if (scheduler_get_first_ready_thread( &next ) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	
	if (AppendFila2(executing, (void *)next)) return LINE_OPERATION_ERROR;
	
	printf("swap started\n");
	if (swapcontext(&(executing_thread->context), &(next->context) ) != SUCCESS_CODE) return CONTEXT_ERROR;
	printf("swap finished\n");

	return SUCCESS_CODE;

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

int scheduler_get_first_ready_thread( TCB_t** next ) {

	// aqui eu vou catar direitinho conforme prioridades
	
	if ( ready == NULL ) return NULL_POINTER;
	if ( next == NULL ) return NULL_POINTER;
	if ( FirstFila2(ready) != SUCCESS_CODE ) return EMPTY_LINE;
	
	
	*next = ready->first->node;

	return SUCCESS_CODE;
	
}



int scheduler_insert_in_ready(TCB_t *thread) {

	// AQUI POSSO PEGAR O SWITCH QUE O GABI JA FEZ MAS NAO FOI MERGED, por enquato deixei soh assim pra eu testar
	
	if (ready == NULL) return NULL_POINTER;
	
	return AppendFila2(ready, (void *) thread);

}
