
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
	return 0;
}

void* func2(void *i) {
	printf("Func 2 has been executed lalalallalalal\n");
	
	//show_state_queues();
	cyield();
	return 0;
}


int main(int argc, char **argv) {
	int id0, id1;
	int i = 10;

	id0 = ccreate(func1, (void *)&i, MEDIUM_PRIO);
	id1 = ccreate(func2, (void *)&i, HIGH_PRIO);

	printf("Resultado da criacao = %d \n", id0);
	printf("Resultado da criacao = %d \n", id1);

	//show_state_queues();

	cyield(); // main cede e deve ir para apto;

	

	printf("Main retornando para terminar o programa\n");
	exit(0);
}

