#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

PFILA2 executing;

int scheduler_block_thread(TCB_t *thread, csem_t *sem) {


	if (executing == NULL) return NULL_POINTER;
	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;
	
	if (FirstFila2(executing) != SUCCESS_CODE) return EMPTY_LINE;
	
	TCB_t *executing_thread = (TCB_t *) executing->first->node;
	executing_thread->state = PROCST_BLOQ;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;		
	if (AppendFila2(sem->fila, (void *)executing_thread)) return LINE_OPERATION_ERROR;

	return SUCCESS_CODE;

}


void test_scheduler_block_thread() {

	csem_t *sem = malloc(sizeof(csem_t));
	PFILA2 fila = malloc(sizeof(FILA2));
	executing = malloc(sizeof(FILA2));

	TCB_t *thread = malloc(sizeof(TCB_t));
	thread->state = PROCST_EXEC;	

	assert(AppendFila2(executing, (void *)thread) == SUCCESS_CODE);
	
	sem->fila = fila;
	sem->count = 10;

	assert(thread->state == PROCST_EXEC);
	assert(FirstFila2(fila) != SUCCESS_CODE);
	assert(FirstFila2(sem->fila) != SUCCESS_CODE);
	assert(FirstFila2(executing) == SUCCESS_CODE);

	assert( scheduler_block_thread(thread, sem) == SUCCESS_CODE);
	assert(thread->state == PROCST_BLOQ);
	assert(FirstFila2(fila) == SUCCESS_CODE);
	assert(FirstFila2(sem->fila) == SUCCESS_CODE);
	assert(FirstFila2(executing) != SUCCESS_CODE);
	assert(executing != NULL);
	assert(fila != NULL);
	assert(sem->fila != NULL);


}


int main(int argc, char *argv[]) {


	test_scheduler_block_thread();
	return 0;
}

