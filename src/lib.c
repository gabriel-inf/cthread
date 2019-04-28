
#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

#define SUCCESS_CODE 0
#define ERROR_CODE -1

int ccreate (void* (*start)(void*), void *arg, int prio) {
	return -1;
}

int csetprio(int tid, int prio) {
	return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {


	printf("INICIALIZANDO O SEMAFORO");
	PFILA2 pfila;
	if (CreateFila2(pfila)) {
		
		*sem = (csem_t) { pfila, count};
		printf("INICIALIZANDO O SEMAFORO COM SUCESSO");
		return SUCCESS_CODE;
	} else {
		printf("INICIALIZANDO O SEMAFORO COM ERRO");
		return ERROR_CODE;
	}
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	printf("Estou rodando com ");
	printf("Linux porque é o jeito");
	printf("\n");
	return 0;
}


