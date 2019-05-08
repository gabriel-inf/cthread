
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

void* func1(void *arg) {
	printf("Func1 has been executed lalallalalalala\n");
	assert( SUCCESS_CODE == csignal(semaphore_test));
	assert( FirstFila2(semaphore_test->fila) != SUCCESS_CODE);
	assert( FirstFila2(ready_high) == SUCCESS_CODE);
	assert( ((TCB_t *)GetAtIteratorFila2(ready_high))->tid == 2);
	printf("testes passaram\n");
	assert(semaphore_test->count == 0);
	cwait(semaphore_test);
	//return;
}

void* func2(void *i) {
	printf("Func 2 has been executed lalalallalalal\n");
	cwait(semaphore_test);
	printf("Func 2 has been FINISHED\n");
}

void *function() {

	printf("thread sendo executada\n");
	return;

}

void teste_create_context() {

	ucontext_t *current_context = (ucontext_t*) malloc(sizeof(ucontext_t));
	

}

int test_ccreate() {

	int i = 2;
	assert (ccreate(function, (void *)&i, 0) > 0);
	return 0;

}

int main(int argc, char **argv) {
	int id0, id1;
	int i = 10;

	id0 = 10;
	id1 = 20;

	semaphore_test = malloc(sizeof(csem_t));
	
	if (csem_init(semaphore_test, 1) != SUCCESS_CODE) {
		printf("NUM DEU, COMPANHEIRO\n");
	}

	id0 = ccreate(func1, (void *)&i, MEDIUM_PRIO);
	id1 = ccreate(func2, (void *)&i, HIGH_PRIO);

	printf("resultado da criacao = %d \n", id0);
	printf("resultado da criacao = %d \n", id1);

	cyield();

	//printf("resultado do retorno = %d \n", scheduler_schedule_next_thread());
	csignal(semaphore_test);
	cyield();

	printf("Main retornando para terminar o programa\n");
	exit(0);
}

