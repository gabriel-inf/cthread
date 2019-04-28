
/*
 *	Programa de exemplo de uso da biblioteca cthread
 *
 *	Vers�o 1.0 - 14/04/2016
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


int inicia(csem_t *sem, int count) {

	PFILA2 pfila = malloc(sizeof(FILA2));
	 
	sem->count = count;
	sem->fila = pfila;
	
	if (CreateFila2(pfila) == SUCCESS_CODE) {
		return SUCCESS_CODE;
	} else {
		return ERROR_CODE;
	}
}

int espera(csem_t *sem) {

	sem->count --;
	if (sem->count < 0) {
		
		if ( FirstFila2(executing) != SUCCESS_CODE ) return ERROR_CODE;

		TCB_t *executing_process = (TCB_t *)executing->first->node;
		executing_process->state = PROCST_BLOQ;

		if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return ERROR_CODE;
		printf("deu certo delecao do executando\n");
		
		if (AppendFila2(sem->fila, &executing_process) != SUCCESS_CODE) return ERROR_CODE;
		printf("deu certo append do executando na fila do semaphoro\n");

	}
	return SUCCESS_CODE;
}

int sinal(csem_t *sem) {

	sem->count ++;
	if (sem->count <= 0) {

		TCB_t *process_to_wake = (TCB_t *) sem->fila->first->node;
		if (FirstFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
		if (DeleteAtIteratorFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
		if (AppendFila2(ready, &process_to_wake) != SUCCESS_CODE) return ERROR_CODE;

	}

	return SUCCESS_CODE;

}

int test_csem_init(int test_sem_count) {

	csem_t *semaphore = malloc(sizeof(csem_t));
	if (inicia(semaphore, test_sem_count) != SUCCESS_CODE) return ERROR_CODE;
	if (semaphore->count != test_sem_count) return ERROR_CODE;
	if (semaphore->fila == NULL) return ERROR_CODE;
	
	return SUCCESS_CODE;
}

int main(int argc, char *argv[]) {

	/*int	id0, id1;
	int i;

	id0 = ccreate(func0, (void *)&i, 0);
	id1 = ccreate(func1, (void *)&i, 0);

	printf("Eu sou a main ap�s a cria��o de ID0 e ID1\n");

	cjoin(id0);
	cjoin(id1);*/

	//printf("Eu sou a main voltandpara terminar o\n");
	
	printf("%d\n", test_csem_init(10));
	printf("%d\n", test_csem_init(-10));	
	printf("%d\n", test_csem_init(0));
	

	/*
	TCB_t *tcb_teste = malloc(sizeof(TCB_t));
	tcb_teste->prio = 1;
	tcb_teste->state = PROCST_EXEC;
	tcb_teste->tid = 10;
	ucontext_t test_context;
	tcb_teste->context = test_context;

	csem_t *semaphore = malloc(sizeof(csem_t));
	printf("aloooooooọ\n");

	if (inicia(semaphore, -10) == SUCCESS_CODE ) {

		printf("deu certo inicializacao do semaphoro\n");

	}
	
	ready = malloc(sizeof(FILA2));
	executing = malloc(sizeof(FILA2));
	if (CreateFila2(executing) == SUCCESS_CODE) {

		printf("deu certo creation\n");

		if ( AppendFila2(executing, (void *)tcb_teste) == SUCCESS_CODE ) {

			printf("deu certo append\n");
			if ( espera(semaphore) == SUCCESS_CODE ) {
				printf("deu certo espera\n");

			}
		}  
	}

	if (CreateFila2(ready) == SUCCESS_CODE) {

		printf("deu certo creation ready\n");

		if (sinal(semaphore) == SUCCESS_CODE) {
			
			printf("deu certo sinal\n");

		}
	}*/


	return 0;

}

