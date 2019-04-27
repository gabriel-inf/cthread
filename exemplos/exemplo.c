
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
#include <stdio.h>
#include <stdlib.h>
#define SUCCESS_CODE 0
#define ERROR_CODE 10

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

int main(int argc, char *argv[]) {

	/*int	id0, id1;
	int i;

	id0 = ccreate(func0, (void *)&i, 0);
	id1 = ccreate(func1, (void *)&i, 0);

	printf("Eu sou a main ap�s a cria��o de ID0 e ID1\n");

	cjoin(id0);
	cjoin(id1);*/

	//printf("Eu sou a main voltandpara terminar o\n");
	printf("Como Assim\n");

	csem_t *semaphore = malloc(sizeof(csem_t));
	printf("aloooooooọ\n");

	int result = inicia(semaphore, 100);
	printf("Resultado inicializacao do semaforo =\n");
	printf("%d\n", result);	
	printf("%d\n", semaphore->count);

	return 0;

}

