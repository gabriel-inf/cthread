
/*
 *	Programa de exemplo de uso da biblioteca cthread
 *
 *	Versão 1.0 - 14/04/2016
 *
 *	Sistemas Operacionais I - www.inf.ufrgs.br
 *
 */

#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include <stdio.h>
#include <stdlib.h>

// Conforme espec, erro eh negativo e sucesso eh zero

#define SUCCESS_CODE 0
#define ERROR_CODE -10 

PFILA2 executing, ready;

/*
void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return 0;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
    return 0;
}*/

int test_csem_init(int test_sem_count) {

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (csem_init(semaphore, test_sem_count) != SUCCESS_CODE) return ERROR_CODE;
	if (semaphore->count != test_sem_count) return ERROR_CODE;
	if (semaphore->fila == NULL) return ERROR_CODE;
	if (FirstFila2(semaphore->fila) == SUCCESS_CODE) return ERROR_CODE; //assert fila sem item
	
	return SUCCESS_CODE;
}

int test_cwait_count_smaller() {

	TCB_t *tcb_teste = malloc(sizeof(TCB_t));
	tcb_teste->prio = 1;
	tcb_teste->state = PROCST_EXEC;
	tcb_teste->tid = 10;
	ucontext_t test_context;
	tcb_teste->context = test_context;

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (csem_init(semaphore, -10) != SUCCESS_CODE) return ERROR_CODE;

	executing = malloc(sizeof(FILA2));
	if (CreateFila2(executing) != SUCCESS_CODE) return ERROR_CODE;
	if (AppendFila2(executing, (void *)tcb_teste) != SUCCESS_CODE) return ERROR_CODE;
	if (cwait(semaphore) != SUCCESS_CODE) return ERROR_CODE;
	
	if (semaphore->count != -11) return ERROR_CODE;

	if (FirstFila2(semaphore->fila) != SUCCESS_CODE) return ERROR_CODE; //assert fila com item
	if (FirstFila2(executing) == SUCCESS_CODE) return ERROR_CODE; //assert fila sem item
	if (tcb_teste->state != PROCST_BLOQ) return ERROR_CODE;
	
	return SUCCESS_CODE;

}

int test_cwait_count_greater() {

	TCB_t *tcb_teste = malloc(sizeof(TCB_t));
	tcb_teste->prio = 1;
	tcb_teste->state = PROCST_EXEC;
	tcb_teste->tid = 10;
	ucontext_t test_context;
	tcb_teste->context = test_context;

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (csem_init(semaphore, 10) != SUCCESS_CODE) return ERROR_CODE;

	executing = malloc(sizeof(FILA2));
	if (CreateFila2(executing) != SUCCESS_CODE) return ERROR_CODE;
	if (AppendFila2(executing, (void *)tcb_teste) != SUCCESS_CODE) return ERROR_CODE;
	if (cwait(semaphore) != SUCCESS_CODE) return ERROR_CODE;
	
	if (semaphore->count != 9) return ERROR_CODE;

	if (FirstFila2(semaphore->fila) == SUCCESS_CODE) return ERROR_CODE; //assert fila sem item
	if (FirstFila2(executing) != SUCCESS_CODE) return ERROR_CODE; //assert fila com item
	if (tcb_teste->state != PROCST_EXEC) return ERROR_CODE;
	
	return SUCCESS_CODE;

}

int test_csignal_count_smaller() {

	TCB_t *tcb_teste = malloc(sizeof(TCB_t));
	tcb_teste->prio = 1;
	tcb_teste->state = PROCST_BLOQ;
	tcb_teste->tid = 10;
	ucontext_t test_context;
	tcb_teste->context = test_context;

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (csem_init(semaphore, -10) != SUCCESS_CODE) return ERROR_CODE;

	ready = malloc(sizeof(FILA2));
	if (CreateFila2(ready) != SUCCESS_CODE) return ERROR_CODE;

	if (AppendFila2(semaphore->fila, (void *)tcb_teste) != SUCCESS_CODE) return ERROR_CODE;
	if (csignal(semaphore) != SUCCESS_CODE) return ERROR_CODE;

	if (semaphore->count != -9) return ERROR_CODE;
	if (tcb_teste->state != PROCST_APTO) return ERROR_CODE;

	if (FirstFila2(semaphore->fila) == SUCCESS_CODE) return ERROR_CODE; //assert fila sem item
	if (FirstFila2(ready) != SUCCESS_CODE) return ERROR_CODE; //assert fila com item

	return SUCCESS_CODE;

}

int test_csignal_count_greater() {

	TCB_t *tcb_teste = malloc(sizeof(TCB_t));
	tcb_teste->prio = 1;
	tcb_teste->state = PROCST_BLOQ;
	tcb_teste->tid = 10;
	ucontext_t test_context;
	tcb_teste->context = test_context;

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (csem_init(semaphore, 10) != SUCCESS_CODE) return ERROR_CODE;

	ready = malloc(sizeof(FILA2));;
	if (CreateFila2(ready) != SUCCESS_CODE) return ERROR_CODE;

	if (AppendFila2(semaphore->fila, (void *)tcb_teste) != SUCCESS_CODE) return ERROR_CODE;
	if (csignal(semaphore) != SUCCESS_CODE) return ERROR_CODE;

	if (semaphore->count != 11) return ERROR_CODE;
	if (tcb_teste->state != PROCST_BLOQ) return ERROR_CODE;

	if (FirstFila2(semaphore->fila) != SUCCESS_CODE) return ERROR_CODE;
	if (FirstFila2(ready) == SUCCESS_CODE) return ERROR_CODE;

	return SUCCESS_CODE;
}


int main(int argc, char *argv[]) {

	/*int	id0, id1;
	int i;

	id0 = ccreate(func0, (void *)&i, 0);
	id1 = ccreate(func1, (void *)&i, 0);

	printf("Eu sou a main após a criação de ID0 e ID1\n");

	cjoin(id0);
	cjoin(id1);*/

	//printf("Eu sou a main voltandpara terminar o\n");
	
	printf("%d\n", test_csem_init(10));
	printf("%d\n", test_csem_init(-10));	
	printf("%d\n", test_csem_init(0));
	printf("%d\n", test_cwait_count_smaller());
	printf("%d\n", test_cwait_count_greater());
	printf("%d\n", test_csignal_count_smaller());
	printf("%d\n", test_csignal_count_greater());


	return 0;

}

