
#include <stdio.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

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
	return -1;
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


