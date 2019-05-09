
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

void* func4(void *arg) {
	printf("Func 4 has been executed\n");
	printf("Func4 has been terminated\n");
	return 0;
}

void* func3(void *arg) {
	printf("Func3 has been executed\n");
	ccreate(func4, (void *) NULL, MEDIUM_PRIO);
	return 0;
}

void* func1(void *arg) {
	printf("Func1 has been executed\n");
	assert(cjoin(4) == SUCCESS_CODE);
	printf("Func1 came back\n");
	return 0;
}

void* func2(void *i) {
	int id0;	
	printf("Func 2 has been executed\n");
	assert(cyield());
	printf("Func 2 came back\n");
	id0 = ccreate(func3, (void*) NULL, MEDIUM_PRIO);
	assert(id0 > 0);
	printf("Resultado da criacao = %d \n", id0);
	printf("Func 2 terminated\n");
	return 0;
}


int main(int argc, char **argv) {
	int id0, id1;
	int i = 10;

	id0 = ccreate(func1, (void *)&i, HIGH_PRIO);
	id1 = ccreate(func2, (void *)&i, MEDIUM_PRIO);

	printf("Resultado da criacao = %d \n", id0);
	printf("Resultado da criacao = %d \n", id1);

	assert( ccreate(NULL, (void *)&i, MEDIUM_PRIO) == NULL_POINTER );

	assert(cyield() == SUCCESS_CODE); // main cede e deve ir para apto;
	assert(cyield() == SUCCESS_CODE);
	assert(cyield() == SUCCESS_CODE);
	assert(cyield() == SUCCESS_CODE);
	assert(cyield() == SUCCESS_CODE);
	

	printf("Main retornando para terminar o programa\n");
	exit(0);
}

