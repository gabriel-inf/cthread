
//
// Programa de teste para primitivas de criação e sincronização
//
// Disclamer: este programa foi desenvolvido para auxiliar no desenvolvimento
//            de testes para o micronúcleo. NÃO HÁ garantias de estar correto.

#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ucontext.h>

csem_t *semaphore_test;

void reset_states() {
	

	ready_low = NULL;
	ready_medium = NULL;
	ready_high = NULL;
	executing = NULL;
	executing = NULL;
	executing = NULL;
	executing = NULL;
	executing = NULL;
	
}


void* func4(void *arg) {
	printf("  -Func4 executed\n");
	return 0;
}

void* func3(void *arg) {
	printf("  -Func3 executed\n");
	ccreate(func4, (void *) NULL, MEDIUM_PRIO);
	return 0;
}

void* func1(void *arg) {

	int tid = *(int *)arg;

	printf("\nveio %d\n", tid);

	printf("  -Func1 executed\n");
	assert (FirstFila2(executing) == SUCCESS_CODE);
	TCB_t *thread = GetAtIteratorFila2(executing);
	printf("thread id : %d \n", thread->tid);	
	assert (thread->tid == tid); //main is in the executing queue

	cjoin(4);

	assert (thread->tid == tid); //main is in the executing queue

	return 0;
}

void* func2(void *i) {
	int id0;	

	printf("  -Func2 executed\n");

	cyield();

	ccreate(func3, (void*) NULL, MEDIUM_PRIO);

	return 0;
}

/*tests cases*/

/**
 * I create two threads and should receive two positive values for them
 */
void testCcreate() {
	
	reset_states();

	printf("Test case: testCcreate ");

	int id0, id1;
	int *i = &id0;
	int *j = &id1;
	id0 = ccreate(func1, (void *)i, HIGH_PRIO);
	id1 = ccreate(func2, (void *)j, MEDIUM_PRIO);

	assert(id0 > 0);
	assert(id1 > 0);
	printf("-> success!\n");

}

/**
 * I create two threads, call cyield and func1 and func2 should execute before ending
 */
void testCcreateCyield() {
	reset_states();
	printf("Test case: testCcreateCyield \n");

	int id0, id1;

	// para poder acessar o id da thread dentro da thread
	int *i = &id0;
	int *j = &id1;

	id0 = ccreate(func1, (void *)i, HIGH_PRIO);
	id1 = ccreate(func2, (void *)j, MEDIUM_PRIO);

	printf("- new tid: %d", id0); 
	printf("- new tid: %d", id1); 

	cyield();
	
	assert (FirstFila2(ready_low) != SUCCESS_CODE);
	assert (FirstFila2(ready_medium) != SUCCESS_CODE);
	assert (FirstFila2(ready_high) != SUCCESS_CODE);
	assert (FirstFila2(executing) == SUCCESS_CODE);
	printf("-> success!\n");


}

/**
 * I create one thread and crete another from this one
 */
void testCcreateCyieldChain() {

	reset_states();

	printf("Test case: testCcreateCyieldChain \n");

	int id0, id1;
	int i = 10;

	id0 = ccreate(func3, (void *)&i, HIGH_PRIO);
	

	assert(cyield() == SUCCESS_CODE);
	assert (FirstFila2(ready_low) != SUCCESS_CODE);
	assert (FirstFila2(ready_medium) != SUCCESS_CODE);
	assert (FirstFila2(ready_high) != SUCCESS_CODE);
	assert (FirstFila2(executing) == SUCCESS_CODE);

	TCB_t *thread = GetAtIteratorFila2(executing);
	assert (thread->tid == 0); //main is in the executing queue

	printf("-> success!\n");

}

/**
 * I create thread and wait with cjoin
 */
void ccreateCwaitCsignal() {

	reset_states();

	printf("Test case: testCcreateCyieldChain \n");

	int id0, id1;
	int i = 10;

	id0 = ccreate(func3, (void *)&i, HIGH_PRIO);
	

	assert(cyield() == SUCCESS_CODE);
	assert (FirstFila2(ready_low) != SUCCESS_CODE);
	assert (FirstFila2(ready_medium) != SUCCESS_CODE);
	assert (FirstFila2(ready_high) != SUCCESS_CODE);
	assert (FirstFila2(executing) == SUCCESS_CODE);

	TCB_t *thread = GetAtIteratorFila2(executing);
	assert (thread->tid == 0); //main is in the executing queue

	printf("-> success!\n");

}




int main(int argc, char **argv) {
	printf("-- Started Integration test --\n");


	testCcreate();
	testCcreateCyield();
	testCcreateCyieldChain();
	char *name = (char*) malloc(sizeof(char));	
	cidentify(name, 90);
	printf("%s\n", name);
	
/*	int id0, id1;*/
/*	int i = 10;*/

/*	id0 = ccreate(func1, (void *)&i, HIGH_PRIO);*/
/*	id1 = ccreate(func2, (void *)&i, MEDIUM_PRIO);*/

/*	assert(id0 > 0);*/
/*	assert(id1 > 0);*/
/*	assert (FirstFila2(executing) == SUCCESS_CODE);*/

/*	TCB_t *thread = GetAtIteratorFila2(executing);*/
/*	assert (thread->tid == 0); //there is something in the executing queue*/




/*	cyield(); // main cede e deve ir para apto;*/
	printf("\nResult: ");
	printf("All integration tests passed!\n");
	exit(0);
}

