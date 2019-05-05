
//
// Programa de teste para primitivas de criação e sincronização
//
// Disclamer: este programa foi desenvolvido para auxiliar no desenvolvimento
//            de testes para o micronúcleo. NÃO HÁ garantias de estar correto.

#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>


void* func1(void *arg) {
	printf("Func1 has been executed\n");
	return 0;
}

void* func2(void *i) {
	int n;
	n = *(int *)i;
	printf("Func 2 has been executed with number %d \n", n);
	return 0;
}

int main(int argc, char **argv) {
	int id0, id1;
	int i = 10;

	id0 = 1;
	id1 = 2;

	id0 = ccreate(func1, (void *)&i, 1);
/*	id1 = ccreate(func2, (void *)&i, 0);*/

/*	printf("Threads fatorial e Fibonnaci criadas...\n");*/

	printf("%d \n", id0);
	printf("%d \n", id1);

	//cjoin(id0);
	//cjoin(id1);

	printf("Main retornando para terminar o programa\n");
}

